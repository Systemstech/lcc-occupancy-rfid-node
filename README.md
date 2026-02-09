# ESP32 LCC Occupancy Detection + RFID Node

This project implements an LCC (Layout Command Control) node using OpenMRNLite on ESP32 with:
- IR beam-break occupancy detection
- PN532 RFID reader for train car identification
- CAN bus communication via MCP2515

## Hardware Requirements

- ESP32 development board
- SN65HVD230 CAN transceiver module
- Two IR beam-break sensors (e.g., E18-D80NK or similar) for direction detection
- PN532 RFID reader module (SPI version)
- 12V to 5V power supply for CAN bus (if needed)
- Jumper wires and breadboard

## Wiring Diagram

### SN65HVD230 CAN Transceiver
```
ESP32          SN65HVD230
GPIO5    -->   CTX (CAN TX)
GPIO4    -->   CRX (CAN RX)
GND      -->   GND
3.3V     -->   VCC
```

### PN532 RFID Reader (SPI)
```
ESP32          PN532
GPIO18   -->   SCK
GPIO19   -->   MISO
GPIO23   -->   MOSI
GPIO15   -->   SS/CS
GPIO16   -->   RSTPDN (optional - for reset)
GPIO14   -->   IRQ (optional - for interrupt)
GND      -->   GND
3.3V     -->   VCC
```

**Important:** Set the PN532 to SPI mode using the DIP switches:
- Switch 1: ON
- Switch 2: OFF

### IR Beam-Break Sensors (Dual for Direction Detection)
```
ESP32          IR Sensor A          IR Sensor B
GPIO12   -->   OUT/Signal           
GPIO13   -->                        OUT/Signal
GND      -->   GND                  GND
5V       -->   VCC                  VCC
```

**Sensor Placement:**
Place sensors 2-6 inches apart along the track:
```
[Sensor A] ----2-6"---- [Sensor B] -----> Direction of travel
   (Entry)                 (Exit)
```

This spacing allows the system to detect which sensor triggers first, determining train direction.

### CAN Bus Connection
```
SN65HVD230     LCC CAN Bus
CANH     -->   CANH
CANL     -->   CANL
GND      -->   GND
```

**Note:** The SN65HVD230 is a 3.3V CAN transceiver that connects directly to the ESP32's built-in TWAI (CAN) controller. No external SPI interface is needed for the CAN connection.

## Software Setup

### Using PlatformIO (Recommended)

1. Install PlatformIO IDE or CLI
2. Create a new project folder
3. Copy `platformio.ini` to the project root
4. Copy `lcc_occupancy_rfid_node.ino` to `src/main.cpp`
5. Build and upload:
   ```bash
   pio run --target upload
   pio device monitor
   ```

### Using Arduino IDE

1. Install ESP32 board support
2. Install required libraries:
   - OpenMRNLite (from GitHub: bakerstu/openmrn)
   - Adafruit PN532
   - Adafruit BusIO
3. Open `lcc_occupancy_rfid_node.ino`
4. Select your ESP32 board
5. Upload

## Configuration

### Node ID
Change the Node ID in the code to a unique value for your layout:
```cpp
#define NODE_ID 0x050101013F00ULL  // Change this!
```

### LCC Event IDs
Customize the event IDs for your layout:
```cpp
const uint64_t EVENT_OCCUPIED = 0x0501010100000001ULL;          // Any sensor occupied
const uint64_t EVENT_CLEAR = 0x0501010100000002ULL;             // Both sensors clear
const uint64_t EVENT_DIRECTION_A_TO_B = 0x0501010100000003ULL;  // Forward/Entering
const uint64_t EVENT_DIRECTION_B_TO_A = 0x0501010100000004ULL;  // Reverse/Exiting
const uint64_t EVENT_SENSOR_A_OCCUPIED = 0x0501010100000005ULL; // Sensor A triggered
const uint64_t EVENT_SENSOR_A_CLEAR = 0x0501010100000006ULL;    // Sensor A cleared
const uint64_t EVENT_SENSOR_B_OCCUPIED = 0x0501010100000007ULL; // Sensor B triggered
const uint64_t EVENT_SENSOR_B_CLEAR = 0x0501010100000008ULL;    // Sensor B cleared
const uint64_t EVENT_RFID_BASE = 0x0501010100010000ULL;         // Base for RFID
```

### Pin Configuration
If your wiring differs, update these definitions:
```cpp
#define CAN_TX_PIN 5           // ESP32 CAN TX to SN65HVD230 (OpenMRNLite default)
#define CAN_RX_PIN 4           // ESP32 CAN RX from SN65HVD230 (OpenMRNLite default)
#define IR_SENSOR_A_PIN 12     // First IR sensor (entry)
#define IR_SENSOR_B_PIN 13     // Second IR sensor (exit)
#define PN532_SS 15            // PN532 chip select
#define PN532_SCK 18           // PN532 SPI clock
#define PN532_MISO 19          // PN532 SPI MISO
#define PN532_MOSI 23          // PN532 SPI MOSI
#define PN532_IRQ 14           // PN532 interrupt (optional)
#define PN532_RESET 16         // PN532 reset (optional)
```

**Note:** OpenMRNLite for ESP32 uses GPIO5/GPIO4 as default CAN pins, not GPIO21/GPIO22.

## How It Works

### Dual Sensor Occupancy Detection with Direction
- **Two IR beam-break sensors** positioned along the track detect train movement
- **Direction Detection**: System determines which sensor triggers first
  - **A→B (Forward/Entering)**: Sensor A triggers, then Sensor B within 2 seconds
  - **B→A (Reverse/Exiting)**: Sensor B triggers, then Sensor A within 2 seconds
- **Individual Sensor Events**: Each sensor sends its own occupied/clear events
- **Overall Occupancy**: Block is occupied if either sensor is triggered
- **Debouncing**: 50ms debouncing prevents false triggers from noise or vibration

**Event Sequence Example (Train entering A→B):**
1. Sensor A triggers → sends `EVENT_SENSOR_A_OCCUPIED` + `EVENT_OCCUPIED`
2. Sensor B triggers → sends `EVENT_SENSOR_B_OCCUPIED` + `EVENT_DIRECTION_A_TO_B`
3. Train passes both sensors
4. Sensor A clears → sends `EVENT_SENSOR_A_CLEAR`
5. Sensor B clears → sends `EVENT_SENSOR_B_CLEAR` + `EVENT_CLEAR`

### RFID Reading
- Scans for RFID tags every 500ms
- When a new tag is detected:
  - Sends an LCC event with ID = `EVENT_RFID_BASE + UID_value`
  - Logs the full UID to serial console
  - Optionally sends datagram with complete UID data
- Clears when tag is removed

### RFID UID Encoding
For 4-byte UIDs (most common):
- UID bytes are combined into a 32-bit value
- Added to `EVENT_RFID_BASE` to create unique event
- Example: UID `0x12 0x34 0x56 0x78` → event offset `0x12345678`

For 7-byte UIDs:
- First 4 bytes used for event offset
- Full UID available in datagram (see serial output)

## LCC Network Integration

This node will:
1. Appear on the LCC bus with the configured Node ID
2. Produce occupancy events that can trigger automation
3. Produce RFID events that identify specific rolling stock
4. Support LCC configuration via CDI (Configuration Definition Information)

### Example Use Cases
- **Direction-aware block signaling** - different signals for entry vs exit
- **Automatic turnout alignment** - set switches based on approach direction
- **Yard sorting by direction** - route trains to different tracks based on entry side
- **Occupancy detection for block signaling** - traditional block detection
- **Train tracking across layout sections** - know which way trains are moving
- **RFID car identification** - identify specific rolling stock
- **Automated station announcements** - "Train arriving from the east"
- **Bi-directional single track management** - control meets and passes
- **Trigger sounds/animations** based on direction and specific cars

## Troubleshooting

### PN532 Not Found
- Check SPI wiring (SCK, MISO, MOSI, SS)
- Verify PN532 is in SPI mode (DIP switches: SW1=ON, SW2=OFF)
- Ensure chip select pin (GPIO15) is correctly connected
- Check that both PN532 and MCP2515 CS pins are on separate GPIOs
- Check power supply (3.3V stable)
- Try adding small delays in initialization

### No CAN Bus Communication
- Verify SN65HVD230 wiring (TX→GPIO5, RX→GPIO4)
- Check CAN bus termination (120Ω resistors at each end)
- Confirm CANH/CANL are not swapped
- Verify 3.3V power to SN65HVD230
- Check CAN bus voltage levels (should see differential signal)
- Ensure ESP32 TWAI pins are GPIO5 and GPIO4 (OpenMRNLite defaults)
- If you need different pins, you'll need to configure them in the ESP32 SDK

### IR Sensors Always Triggered or Not Detecting
- Check both sensor alignments (emitter to detector)
- Verify voltage levels (3.3V vs 5V logic)
- Adjust sensor sensitivity (if available)
- Check for INPUT_PULLUP conflicts
- Ensure sensors are spaced 2-6" apart for reliable direction detection
- Test each sensor individually by blocking beams one at a time
- Verify wiring - Sensor A on GPIO12, Sensor B on GPIO13

### Compile Errors
- Ensure all libraries are installed
- Update to latest ESP32 board package
- Check for OpenMRNLite compatibility

## Serial Monitor Output

Example output:
```
ESP32 LCC Occupancy + RFID Node Starting...
SPIFFS initialized
IR Sensor A initialized on pin 12
IR Sensor B initialized on pin 13
Direction detection enabled
Found PN532 chip version: 1
PN532 initialized and ready to read tags (SPI mode)
LCC Node initialized
Node ID: 0x050101013F00

Sensor A TRIGGERED
Sent LCC Event: 0x0501010100000005
=== BLOCK OCCUPIED ===
Sent LCC Event: 0x0501010100000001

Sensor B TRIGGERED
Sent LCC Event: 0x0501010100000007
Direction detected: A→B (Forward/Entering)
Sent LCC Event: 0x0501010100000003

RFID Tag detected: 04 A3 B2 C1
Sent LCC Event: 0x050101010004A3B2
RFID Datagram: FF DF 04 04 A3 B2 C1

Sensor A CLEAR
Sent LCC Event: 0x0501010100000006

Sensor B CLEAR
Sent LCC Event: 0x0501010100000008
=== BLOCK CLEAR ===
Sent LCC Event: 0x0501010100000002
Last direction was: A→B
```

## Advanced Features

### Multiple RFID Readers
To add more readers, modify the code to:
- Create multiple PN532 instances with different CS pins
- Each reader needs its own chip select line
- All readers share the same SPI bus (SCK/MISO/MOSI)
- Assign different event base addresses

### CDI Configuration
The node supports LCC Configuration Definition Information (CDI) for remote configuration of:
- Occupancy events (occupied/clear)
- RFID enable/disable
- RFID event base address

Access via JMRI or other LCC configuration tools.

## License

This code uses OpenMRNLite which is licensed under BSD/GPL.
Adafruit libraries are under BSD license.

## Support

For issues:
- OpenMRNLite: https://github.com/bakerstu/openmrn
- LCC Standards: https://www.openlcb.org/
- PN532: https://learn.adafruit.com/adafruit-pn532-rfid-nfc
