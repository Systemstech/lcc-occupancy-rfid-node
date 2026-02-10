# Wokwi Circuit Diagram
## ESP32 LCC Occupancy + RFID Node

## How to Use This File

1. Go to https://wokwi.com
2. Click "New Project" → "ESP32"
3. Click on "diagram.json" tab
4. Copy the JSON code below and paste it
5. The circuit will appear automatically!

---

## diagram.json for Wokwi

```json
{
  "version": 1,
  "author": "LCC Occupancy + RFID Node",
  "editor": "wokwi",
  "parts": [
    {
      "type": "wokwi-esp32-devkit-v1",
      "id": "esp32",
      "top": 0,
      "left": 0,
      "attrs": {}
    },
    {
      "type": "wokwi-led",
      "id": "ir_emitter_a",
      "top": -96,
      "left": -200,
      "attrs": { "color": "red", "label": "IR Emitter A" }
    },
    {
      "type": "wokwi-photoresistor-sensor",
      "id": "ir_receiver_a",
      "top": -96,
      "left": -100,
      "attrs": { "label": "IR Receiver A" }
    },
    {
      "type": "wokwi-led",
      "id": "ir_emitter_b",
      "top": -96,
      "left": 100,
      "attrs": { "color": "red", "label": "IR Emitter B" }
    },
    {
      "type": "wokwi-photoresistor-sensor",
      "id": "ir_receiver_b",
      "top": -96,
      "left": 200,
      "attrs": { "label": "IR Receiver B" }
    },
    {
      "type": "wokwi-resistor",
      "id": "r1",
      "top": 50,
      "left": -250,
      "attrs": { "value": "330" }
    },
    {
      "type": "wokwi-resistor",
      "id": "r2",
      "top": 100,
      "left": -250,
      "attrs": { "value": "330" }
    }
  ],
  "connections": [
    [ "esp32:TX0", "$serialMonitor:RX", "", [] ],
    [ "esp32:RX0", "$serialMonitor:TX", "", [] ],
    [ "esp32:GND.1", "ir_emitter_a:C", "black", [ "v0" ] ],
    [ "esp32:5V", "r1:1", "red", [ "h0" ] ],
    [ "r1:2", "ir_emitter_a:A", "red", [ "v0" ] ],
    [ "ir_receiver_a:VCC", "esp32:5V", "red", [ "v0" ] ],
    [ "ir_receiver_a:GND", "esp32:GND.1", "black", [ "v0" ] ],
    [ "ir_receiver_a:OUT", "esp32:GPIO12", "yellow", [ "v0" ] ],
    [ "esp32:GND.2", "ir_emitter_b:C", "black", [ "v0" ] ],
    [ "esp32:5V", "r2:1", "red", [ "h0" ] ],
    [ "r2:2", "ir_emitter_b:A", "red", [ "v0" ] ],
    [ "ir_receiver_b:VCC", "esp32:5V", "red", [ "v0" ] ],
    [ "ir_receiver_b:GND", "esp32:GND.2", "black", [ "v0" ] ],
    [ "ir_receiver_b:OUT", "esp32:GPIO13", "white", [ "v0" ] ]
  ],
  "dependencies": {}
}
```

---

## Notes for Wokwi

### Limitations:
⚠️ Wokwi doesn't have:
- SN65HVD230 CAN transceiver (can't simulate CAN bus)
- PN532 RFID module (can't simulate RFID)
- Actual IR beam-break sensors

### What We Can Simulate:
✅ IR sensors using photoresistors (simulates light detection)
✅ ESP32 GPIO input/output
✅ Basic sensor triggering
✅ Serial monitor output
✅ Code execution

### How to Test in Wokwi:

1. **Add the code:**
   - Click "sketch.ino" tab
   - Paste your Arduino code
   - Comment out CAN and RFID initialization (they won't work in simulation)

2. **Simulate IR sensors:**
   - Click on photoresistor sensors
   - Adjust light level to simulate beam break
   - Dark = beam blocked
   - Light = beam clear

3. **Watch Serial Monitor:**
   - Click "Start Simulation"
   - Open Serial Monitor
   - See sensor triggers and direction detection

---

## Simplified Code for Wokwi Testing

For testing ONLY the IR sensors in Wokwi, use this simplified version:

```cpp
// Simplified for Wokwi - IR sensors only
#define IR_SENSOR_A_PIN 12
#define IR_SENSOR_B_PIN 13
#define IR_ACTIVE_HIGH false

void setup() {
  Serial.begin(115200);
  pinMode(IR_SENSOR_A_PIN, INPUT_PULLUP);
  pinMode(IR_SENSOR_B_PIN, INPUT_PULLUP);
  Serial.println("IR Sensor Test - Wokwi Simulation");
}

void loop() {
  int readingA = digitalRead(IR_SENSOR_A_PIN);
  int readingB = digitalRead(IR_SENSOR_B_PIN);
  
  #if IR_ACTIVE_HIGH
    bool sensorA = (readingA == HIGH);
    bool sensorB = (readingB == HIGH);
  #else
    bool sensorA = (readingA == LOW);
    bool sensorB = (readingB == LOW);
  #endif
  
  if (sensorA) Serial.println("Sensor A: BLOCKED");
  if (sensorB) Serial.println("Sensor B: BLOCKED");
  
  delay(500);
}
```

---

## Full Project Visualization

Since Wokwi can't show CAN or RFID, here's what each component does:

### In Wokwi (Visible):
- ESP32
- IR Sensor A (GPIO12)
- IR Sensor B (GPIO13)
- Serial Monitor output

### Not in Wokwi (Hardware Only):
- SN65HVD230 CAN → GPIO5/4 (connect in real hardware)
- PN532 RFID → GPIO15/18/19/23 (connect in real hardware)
- Actual CAN bus network

---

## Share Your Wokwi Project

After creating:
1. Click "Save" (top right)
2. Click "Share"
3. Copy the link
4. Share with others for collaboration

Example link format:
`https://wokwi.com/projects/XXXXXXXXXXXXXXXXXX`

---

## Alternative: Use Wokwi CLI

For advanced users:
```bash
npm install -g wokwi-cli
wokwi-cli simulate diagram.json
```

This lets you run simulations from command line!

---

Need help setting it up? Let me know which part is unclear!
