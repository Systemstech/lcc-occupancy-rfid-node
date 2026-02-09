/**
 * ESP32 LCC Node - Occupancy Detection with RFID Reader
 * 
 * Hardware:
 * - ESP32 board
 * - SN65HVD230 CAN transceiver (GPIO21=TX, GPIO22=RX)
 * - PN532 RFID reader (SPI: GPIO15=SS, GPIO18=SCK, GPIO19=MISO, GPIO23=MOSI)
 * - Two IR beam-break sensors (GPIO12=Sensor A, GPIO13=Sensor B)
 * 
 * Features:
 * - Dual IR beam-break for direction detection
 * - LCC occupancy detection producer
 * - RFID tag reading with LCC event production
 * - Train direction detection (A→B vs B→A)
 * - Configuration via LCC CDI
 */

#include <Arduino.h>
#include <SPIFFS.h>
#include <SPI.h>
#include <driver/gpio.h>
#include <soc/gpio_sig_map.h>
#include <OpenMRNLite.h>
#include <Adafruit_PN532.h>

// LCC Configuration
#define NODE_ID 0x050101013F00ULL  // Change this to your unique node ID

// Pin Definitions
// CAN Bus - ESP32 built-in TWAI (CAN) controller with SN65HVD230
#define CAN_TX_PIN 21
#define CAN_RX_PIN 22

// IR Sensors (dual for direction detection)
#define IR_SENSOR_A_PIN 12  // First sensor (entry side)
#define IR_SENSOR_B_PIN 13  // Second sensor (exit side)

// SPI for PN532
#define PN532_SCK 18
#define PN532_MISO 19
#define PN532_MOSI 23
#define PN532_SS 15     // PN532 chip select
#define PN532_IRQ 14    // PN532 interrupt (optional)
#define PN532_RESET 16  // PN532 reset (optional)

// OpenMRN objects
openlcb::SimpleCanStack stack(NODE_ID);
openlcb::ConfigDef cfg(0);

// PN532 RFID Reader (SPI)
Adafruit_PN532 nfc(PN532_SS);

// Occupancy state - dual sensors
bool lastSensorAState = false;
bool lastSensorBState = false;
bool currentSensorAState = false;
bool currentSensorBState = false;
unsigned long lastDebounceTimeA = 0;
unsigned long lastDebounceTimeB = 0;
const unsigned long debounceDelay = 50;

// Direction detection
enum Direction {
    DIR_NONE = 0,
    DIR_A_TO_B = 1,  // Forward (e.g., entering)
    DIR_B_TO_A = 2   // Reverse (e.g., exiting)
};

Direction lastDirection = DIR_NONE;
unsigned long firstSensorTime = 0;
const unsigned long directionTimeout = 2000; // 2 seconds to complete direction sequence

// Overall occupancy (either sensor triggered)
bool isOccupied = false;

// RFID state
uint8_t lastUID[7] = {0};
uint8_t lastUIDLength = 0;
unsigned long lastRFIDRead = 0;
const unsigned long rfidReadInterval = 500; // Read RFID every 500ms

// LCC Event IDs - customize these for your layout
const uint64_t EVENT_OCCUPIED = 0x0501010100000001ULL;
const uint64_t EVENT_CLEAR = 0x0501010100000002ULL;
const uint64_t EVENT_DIRECTION_A_TO_B = 0x0501010100000003ULL;  // Forward/Entering
const uint64_t EVENT_DIRECTION_B_TO_A = 0x0501010100000004ULL;  // Reverse/Exiting
const uint64_t EVENT_SENSOR_A_OCCUPIED = 0x0501010100000005ULL;
const uint64_t EVENT_SENSOR_A_CLEAR = 0x0501010100000006ULL;
const uint64_t EVENT_SENSOR_B_OCCUPIED = 0x0501010100000007ULL;
const uint64_t EVENT_SENSOR_B_CLEAR = 0x0501010100000008ULL;
const uint64_t EVENT_RFID_BASE = 0x0501010100010000ULL; // Base for RFID events

// Configuration Definition Information (CDI)
namespace openlcb {
CDI_GROUP(OccupancyConfig);
CDI_GROUP_ENTRY(occupied_event, EventConfigEntry, 
    Name("Occupied Event"), 
    Description("Event produced when occupancy is detected"));
CDI_GROUP_ENTRY(clear_event, EventConfigEntry,
    Name("Clear Event"),
    Description("Event produced when occupancy clears"));
CDI_GROUP_END();

CDI_GROUP(RFIDConfig);
CDI_GROUP_ENTRY(enabled, Uint8ConfigEntry,
    Name("RFID Enabled"),
    Description("1 to enable RFID reading, 0 to disable"),
    Default(1));
CDI_GROUP_ENTRY(rfid_event_base, EventConfigEntry,
    Name("RFID Event Base"),
    Description("Base event ID for RFID tags"));
CDI_GROUP_END();

CDI_GROUP(MainCDI, Segment(MemoryConfigDef::SPACE_CONFIG), Offset(128));
CDI_GROUP_ENTRY(occupancy, OccupancyConfig, Name("Occupancy Detection"));
CDI_GROUP_ENTRY(rfid, RFIDConfig, Name("RFID Reader"));
CDI_GROUP_END();

CDI_GROUP(ConfigDef, MainCdi(MainCDI));
CDI_GROUP_END();
} // namespace openlcb

openlcb::ConfigDef cfg(0);

/**
 * Initialize SPIFFS for configuration storage
 */
void initSPIFFS() {
    if (!SPIFFS.begin(true)) {
        Serial.println("SPIFFS initialization failed!");
        while(1) delay(1000);
    }
    Serial.println("SPIFFS initialized");
}

/**
 * Initialize PN532 RFID reader
 */
void initRFID() {
    // Initialize SPI for PN532
    SPI.begin(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);
    
    nfc.begin();
    
    uint32_t versiondata = nfc.getFirmwareVersion();
    if (!versiondata) {
        Serial.println("PN532 not found - continuing without RFID");
        return;
    }
    
    Serial.print("Found PN532 chip version: ");
    Serial.println((versiondata >> 24) & 0xFF, HEX);
    
    // Configure PN532 to read RFID tags
    nfc.SAMConfig();
    
    Serial.println("PN532 initialized and ready to read tags (SPI mode)");
}

/**
 * Initialize IR beam-break sensors
 */
void initIRSensors() {
    pinMode(IR_SENSOR_A_PIN, INPUT_PULLUP);
    pinMode(IR_SENSOR_B_PIN, INPUT_PULLUP);
    Serial.println("IR Sensor A initialized on pin " + String(IR_SENSOR_A_PIN));
    Serial.println("IR Sensor B initialized on pin " + String(IR_SENSOR_B_PIN));
    Serial.println("Direction detection enabled");
}

/**
 * Send LCC event
 */
void sendEvent(uint64_t eventID) {
    auto *b = stack.node()->iface()->global_message_write_flow()->alloc();
    b->data()->reset(openlcb::Defs::MTI_EVENT_REPORT, stack.node()->node_id(),
                     openlcb::eventid_to_buffer(eventID));
    stack.node()->iface()->global_message_write_flow()->send(b);
    
    Serial.print("Sent LCC Event: 0x");
    Serial.println((unsigned long)eventID, HEX);
}

/**
 * Check occupancy sensors and detect direction
 */
void checkOccupancy() {
    // Read both sensors
    int readingA = digitalRead(IR_SENSOR_A_PIN);
    int readingB = digitalRead(IR_SENSOR_B_PIN);
    
    // IR beam-break sensors typically go LOW when beam is broken (occupied)
    bool sensorATriggered = (readingA == LOW);
    bool sensorBTriggered = (readingB == LOW);
    
    // Debounce Sensor A
    if (sensorATriggered != lastSensorAState) {
        lastDebounceTimeA = millis();
    }
    
    if ((millis() - lastDebounceTimeA) > debounceDelay) {
        if (sensorATriggered != currentSensorAState) {
            currentSensorAState = sensorATriggered;
            
            if (currentSensorAState) {
                Serial.println("Sensor A TRIGGERED");
                sendEvent(EVENT_SENSOR_A_OCCUPIED);
                
                // Check for direction sequence
                if (currentSensorBState) {
                    // B was already triggered, this is B→A direction
                    lastDirection = DIR_B_TO_A;
                    Serial.println("Direction detected: B→A (Reverse/Exiting)");
                    sendEvent(EVENT_DIRECTION_B_TO_A);
                } else {
                    // A triggered first, start timing
                    firstSensorTime = millis();
                }
            } else {
                Serial.println("Sensor A CLEAR");
                sendEvent(EVENT_SENSOR_A_CLEAR);
            }
        }
    }
    
    lastSensorAState = sensorATriggered;
    
    // Debounce Sensor B
    if (sensorBTriggered != lastSensorBState) {
        lastDebounceTimeB = millis();
    }
    
    if ((millis() - lastDebounceTimeB) > debounceDelay) {
        if (sensorBTriggered != currentSensorBState) {
            currentSensorBState = sensorBTriggered;
            
            if (currentSensorBState) {
                Serial.println("Sensor B TRIGGERED");
                sendEvent(EVENT_SENSOR_B_OCCUPIED);
                
                // Check for direction sequence
                if (currentSensorAState && 
                    (millis() - firstSensorTime) < directionTimeout) {
                    // A was triggered first, this is A→B direction
                    lastDirection = DIR_A_TO_B;
                    Serial.println("Direction detected: A→B (Forward/Entering)");
                    sendEvent(EVENT_DIRECTION_A_TO_B);
                }
            } else {
                Serial.println("Sensor B CLEAR");
                sendEvent(EVENT_SENSOR_B_CLEAR);
            }
        }
    }
    
    lastSensorBState = sensorBTriggered;
    
    // Overall occupancy state (either sensor triggered)
    bool nowOccupied = currentSensorAState || currentSensorBState;
    
    if (nowOccupied != isOccupied) {
        isOccupied = nowOccupied;
        
        if (isOccupied) {
            Serial.println("=== BLOCK OCCUPIED ===");
            sendEvent(EVENT_OCCUPIED);
        } else {
            Serial.println("=== BLOCK CLEAR ===");
            sendEvent(EVENT_CLEAR);
            // Reset direction when block clears
            if (lastDirection != DIR_NONE) {
                Serial.print("Last direction was: ");
                Serial.println(lastDirection == DIR_A_TO_B ? "A→B" : "B→A");
            }
            lastDirection = DIR_NONE;
        }
    }
}

/**
 * Read RFID tag and send event with UID
 */
void checkRFID() {
    if (millis() - lastRFIDRead < rfidReadInterval) {
        return; // Not time to read yet
    }
    
    lastRFIDRead = millis();
    
    uint8_t uid[7];
    uint8_t uidLength;
    
    // Check for a tag (non-blocking)
    if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 10)) {
        // Check if this is a new tag or different from last read
        bool newTag = (uidLength != lastUIDLength);
        if (!newTag) {
            for (uint8_t i = 0; i < uidLength; i++) {
                if (uid[i] != lastUID[i]) {
                    newTag = true;
                    break;
                }
            }
        }
        
        if (newTag) {
            Serial.print("RFID Tag detected: ");
            for (uint8_t i = 0; i < uidLength; i++) {
                Serial.print(uid[i], HEX);
                Serial.print(" ");
            }
            Serial.println();
            
            // Store this UID
            lastUIDLength = uidLength;
            memcpy(lastUID, uid, uidLength);
            
            // Create event ID from UID
            // We'll use the base event + UID as offset
            // For 4-byte UID: combine bytes into a 32-bit number
            uint32_t uidValue = 0;
            for (uint8_t i = 0; i < min(uidLength, (uint8_t)4); i++) {
                uidValue = (uidValue << 8) | uid[i];
            }
            
            uint64_t rfidEvent = EVENT_RFID_BASE + uidValue;
            sendEvent(rfidEvent);
            
            // Also send individual datagram with full UID if needed
            // This allows reconstruction of full 7-byte UIDs
            sendRFIDDatagram(uid, uidLength);
        }
    } else {
        // No tag present - clear last UID if it was set
        if (lastUIDLength > 0) {
            Serial.println("RFID Tag removed");
            lastUIDLength = 0;
            memset(lastUID, 0, sizeof(lastUID));
        }
    }
}

/**
 * Send RFID UID as LCC datagram for full data transmission
 */
void sendRFIDDatagram(uint8_t* uid, uint8_t uidLength) {
    // Create a custom datagram with RFID data
    // Format: [0xFF][0xRF][length][uid bytes...]
    uint8_t data[10];
    data[0] = 0xFF; // Custom protocol marker
    data[1] = 0xDF; // RFID Data Frame
    data[2] = uidLength;
    memcpy(&data[3], uid, uidLength);
    
    // For now, we'll just log this - implementing full datagram protocol
    // would require more OpenMRN integration
    Serial.print("RFID Datagram: ");
    for (uint8_t i = 0; i < uidLength + 3; i++) {
        Serial.print(data[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
}

void setup() {
    Serial.begin(115200);
    Serial.println("\nESP32 LCC Occupancy + RFID Node Starting...");
    
    // Initialize file system for configuration
    initSPIFFS();
    
    // Initialize sensors
    initIRSensors();
    initRFID();
    
    // Initialize OpenMRN stack
    stack.create_config_file_if_needed(cfg.seg().internal_data(), 
                                       openlcb::CANONICAL_VERSION, 
                                       openlcb::CONFIG_FILE_SIZE);
    
    // Configure ESP32 TWAI (CAN) pins
    // The ESP32 has a built-in CAN controller (TWAI)
    esp_rom_gpio_connect_out_signal(CAN_TX_PIN, TWAI_TX_IDX, false, false);
    esp_rom_gpio_connect_in_signal(CAN_RX_PIN, TWAI_RX_IDX, false);
    gpio_set_direction((gpio_num_t)CAN_TX_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction((gpio_num_t)CAN_RX_PIN, GPIO_MODE_INPUT);
    
    // Start the LCC stack with built-in CAN interface
    stack.add_can_port_select("/dev/twai/twai0");
    
    // Start the stack
    stack.loop_executor();
    
    Serial.println("LCC Node initialized");
    Serial.print("Node ID: 0x");
    Serial.println((unsigned long)(NODE_ID >> 32), HEX);
    Serial.println((unsigned long)(NODE_ID & 0xFFFFFFFF), HEX);
}

void loop() {
    // Process OpenMRN stack
    stack.loop_executor();
    
    // Check occupancy sensor
    checkOccupancy();
    
    // Check RFID reader
    checkRFID();
    
    // Small delay to prevent overwhelming the bus
    delay(10);
}
