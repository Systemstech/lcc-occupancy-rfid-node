# ESP32 DevKit V1 Pinout Diagram
## With LCC Occupancy + RFID Node Pin Assignments

---

## ESP32 DevKit V1 - 30 Pin Version

```
                                    ╔═══════════════╗
                                    ║   ESP32-WROOM ║
                                    ║   DevKit V1   ║
                                    ╚═══════════════╝
                                          USB
                                           ║
        ┌───────────────────────────────────────────────────────┐
        │                                                         │
   EN   ├─ 1                                                 30 ─┤ GND
        │                                                         │
VP/36  ├─ 2                                                 29 ─┤ VIN
        │                                                         │
VN/39  ├─ 3                                                 28 ─┤ 3V3
        │                                                         │
  34   ├─ 4                                                 27 ─┤ GPIO21 (CAN TX - NOT USED*)
        │                                                         │
  35   ├─ 5                                                 26 ─┤ GPIO22 (CAN RX - NOT USED*)
        │                                                         │
  32   ├─ 6                                                 25 ─┤ RX0
        │                                                         │
  33   ├─ 7                                                 24 ─┤ TX0
        │                                                         │
  25   ├─ 8                                                 23 ─┤ GPIO23 ◄── [PN532 MOSI - PURPLE]
        │                                                         │
  26   ├─ 9                                                 22 ─┤ GPIO22 ◄── [NOT USED - See note*]
        │                                                         │
  27   ├─10                                                 21 ─┤ GND
        │                                                         │
GPIO14 ├─11 ◄── [PN532 IRQ - BROWN]                        20 ─┤ GPIO19 ◄── [PN532 MISO - GREEN]
        │                                                         │
GPIO12 ├─12 ◄── [IR SENSOR A - YELLOW] ★                   19 ─┤ GPIO18 ◄── [PN532 SCK - BLUE]
        │                                                         │
 GND   ├─13                                                 18 ─┤ GPIO5  ◄── [CAN TX - ORANGE] ★
        │                                                         │
GPIO13 ├─14 ◄── [IR SENSOR B - WHITE] ★                    17 ─┤ GPIO17
        │                                                         │
 SD2   ├─15                                                 16 ─┤ GPIO16 ◄── [PN532 RESET - WHITE]
        │                                                         │
 SD3   ├─16                                                 15 ─┤ GPIO4  ◄── [CAN RX - YELLOW] ★
        │                                                         │
 CMD   ├─17                                                 14 ─┤ GPIO0
        │                                                         │
 5V    ├─18 ◄── [5V to IR SENSORS - RED] ★                 13 ─┤ GPIO2
        │                                                         │
        └───────────────────────────────────────────────────────┘

★ = Used in our project
* NOTE: GPIO21/22 labels on PCB are for I2C by default, but we're using GPIO5/4 for CAN
```

---

## Our Project Pin Usage Summary

### ✅ Pins We're Using:

| ESP32 Pin | Function | Component | Wire Color | Notes |
|-----------|----------|-----------|------------|-------|
| **GPIO4** | CAN RX | SN65HVD230 CRX | Yellow | OpenMRNLite default |
| **GPIO5** | CAN TX | SN65HVD230 CTX | Orange | OpenMRNLite default |
| **GPIO12** | IR Sensor A | IR Receiver A Signal | Yellow | Entry sensor |
| **GPIO13** | IR Sensor B | IR Receiver B Signal | White | Exit sensor |
| **GPIO14** | RFID IRQ | PN532 IRQ | Brown | Optional |
| **GPIO15** | SPI CS | PN532 SS | Gray | Chip Select |
| **GPIO16** | RFID Reset | PN532 RSTPDN | White | Optional |
| **GPIO18** | SPI SCK | PN532 SCK | Blue | SPI Clock |
| **GPIO19** | SPI MISO | PN532 MISO | Green | SPI Data In |
| **GPIO23** | SPI MOSI | PN532 MOSI | Purple | SPI Data Out |
| **3.3V** | Power | SN65HVD230, PN532 | Red | 3.3V only! |
| **5V** | Power | IR Sensors | Red | IR emitters/receivers |
| **GND** | Ground | All components | Black | Common ground |

### ❌ Pins We're NOT Using:

All other GPIO pins are available for expansion!

---

## Important Pin Notes

### ⚠️ Input Only Pins (Cannot be used as outputs):
- **GPIO34, GPIO35, GPIO36 (VP), GPIO39 (VN)**
- These can ONLY read input, not drive outputs
- Don't have internal pull-ups

### ⚠️ Strapping Pins (Be careful during boot):
- **GPIO0** - Must be HIGH during boot (LOW = flash mode)
- **GPIO2** - Must be LOW during boot (used for boot mode selection)
- **GPIO12** - Must be LOW during boot (sets voltage for internal regulator)
- **GPIO15** - Must be HIGH during boot

**Our Usage:** We're using GPIO12 and GPIO15, but with INPUT_PULLUP they should boot fine.

### ⚠️ SPI Flash Pins (DO NOT USE):
- **GPIO6, GPIO7, GPIO8, GPIO9, GPIO10, GPIO11**
- Connected to internal flash memory
- Using these will crash the ESP32

### ✅ Safe ADC Pins (if you need analog input later):
- GPIO32, GPIO33, GPIO34, GPIO35, GPIO36, GPIO39
- These can read 0-3.3V analog signals

---

## Power Pins Explained

```
VIN (Pin 29)  - 5V input when powered via external source
              - NOT connected to USB 5V!
              - Use this if powering from 5V adapter

5V (Pin 18)   - 5V output when powered via USB
              - Connected to USB VBUS
              - Use this to power 5V sensors (our IR sensors)

3V3 (Pin 28)  - 3.3V output (regulated from VIN or USB)
              - Max ~500mA
              - Use for 3.3V devices (our CAN/RFID)

GND (Pins 13, 21, 30) - All connected together
              - Connect ALL grounds to any of these
```

---

## Pin Grouping by Function

### CAN Bus (OpenMRNLite Defaults)
```
GPIO5  ──→ CTX (Transmit)
GPIO4  ──→ CRX (Receive)
```

### SPI Bus (PN532 RFID)
```
GPIO18 ──→ SCK  (Clock)
GPIO19 ──→ MISO (Data In)
GPIO23 ──→ MOSI (Data Out)
GPIO15 ──→ CS   (Chip Select)
GPIO16 ──→ RST  (Reset - optional)
GPIO14 ──→ IRQ  (Interrupt - optional)
```

### IR Sensors (Digital Input)
```
GPIO12 ──→ Sensor A Signal
GPIO13 ──→ Sensor B Signal
```

---

## Available for Future Expansion

These GPIO pins are still free:

```
GPIO0  - Available (but strapping pin - use carefully)
GPIO2  - Available (but strapping pin - use carefully)
GPIO17 - Available ✓
GPIO21 - Available (I2C SDA if needed)
GPIO22 - Available (I2C SCL if needed)
GPIO25 - Available ✓
GPIO26 - Available ✓
GPIO27 - Available ✓
GPIO32 - Available (ADC1_CH4) ✓
GPIO33 - Available (ADC1_CH5) ✓
GPIO34 - Input only (ADC1_CH6)
GPIO35 - Input only (ADC1_CH7)
GPIO36 - Input only (ADC1_CH0, VP)
GPIO39 - Input only (ADC1_CH3, VN)
```

**Ideas for expansion:**
- More IR sensor pairs (use GPIO17, GPIO25, GPIO26, GPIO27)
- I2C sensors (use GPIO21/22 for SDA/SCL)
- Analog sensors (use GPIO32-39)
- LED indicators (any available GPIO)
- Buttons (any available GPIO with INPUT_PULLUP)

---

## Visual Quick Reference

```
LEFT SIDE (Pins 1-18):               RIGHT SIDE (Pins 30-13):
                                     
EN          [Not Used]               GND         [Ground] ★
GPIO36/VP   [Not Used]               VIN         [Not Used]
GPIO39/VN   [Not Used]               3V3         [RFID/CAN Power] ★
GPIO34      [Not Used]               GPIO21      [Available - I2C SDA]
GPIO35      [Not Used]               GPIO22      [Available - I2C SCL]
GPIO32      [Not Used]               RX0         [Serial Debug]
GPIO33      [Not Used]               TX0         [Serial Debug]
GPIO25      [Not Used]               GPIO23      [PN532 MOSI] ★
GPIO26      [Not Used]               GPIO22      [Available]
GPIO27      [Not Used]               GND         [Ground] ★
GPIO14      [PN532 IRQ] ★            GPIO19      [PN532 MISO] ★
GPIO12      [IR Sensor A] ★          GPIO18      [PN532 SCK] ★
GND         [Ground] ★               GPIO5       [CAN TX] ★
GPIO13      [IR Sensor B] ★          GPIO17      [Available]
SD2/GPIO9   [Flash - Don't Use]      GPIO16      [PN532 RST] ★
SD3/GPIO10  [Flash - Don't Use]      GPIO4       [CAN RX] ★
CMD/GPIO11  [Flash - Don't Use]      GPIO0       [Available - Strapping]
5V          [IR Sensor Power] ★      GPIO2       [Available - Strapping]
```

---

## Common Mistakes to Avoid

❌ **Connecting 5V to PN532 or SN65HVD230** → Use 3.3V only!
❌ **Using GPIO6-11** → These are for flash, will crash ESP32
❌ **Forgetting pull-ups on strapping pins** → May prevent boot
❌ **Mixing up GPIO numbers with physical pin numbers** → Always use GPIO numbers
❌ **Not connecting all grounds together** → Will cause erratic behavior

---

## Testing Individual Pins

To test if a pin works:

```cpp
void setup() {
  pinMode(12, OUTPUT);  // Test GPIO12
}

void loop() {
  digitalWrite(12, HIGH);
  delay(500);
  digitalWrite(12, LOW);
  delay(500);
}
```

Connect LED + resistor to pin, should blink!

---

## Datasheets & References

- ESP32 Datasheet: https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf
- ESP32 Pinout Reference: https://randomnerdtutorials.com/esp32-pinout-reference-gpios/
- ESP32 Hardware Design: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/hw-reference/

---

Need help with a specific pin or function? Let me know!
