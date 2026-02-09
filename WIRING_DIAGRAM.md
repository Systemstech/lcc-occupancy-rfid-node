# Visual Wiring Guide

## Complete System Diagram

```
                                    ┌─────────────────┐
                                    │     ESP32       │
                                    │                 │
                                    │  GPIO5 (TX) ────┼──> SN65HVD230 CTX
                                    │  GPIO4 (RX) <───┼─── SN65HVD230 CRX
                                    │                 │
                                    │  GPIO18 (SCK) ──┼──> PN532 SCK
                                    │  GPIO19 (MISO)<─┼─── PN532 MISO
                                    │  GPIO23 (MOSI)──┼──> PN532 MOSI
                                    │  GPIO15 (CS) ───┼──> PN532 SS
                                    │  GPIO16 ────────┼──> PN532 RST (opt)
                                    │  GPIO14 <───────┼─── PN532 IRQ (opt)
                                    │                 │
                                    │  GPIO12 <───────┼─── IR Sensor A OUT
                                    │  GPIO13 <───────┼─── IR Sensor B OUT
                                    │                 │
                                    │  3.3V ──────────┼──> SN65HVD230 VCC
                                    │  3.3V ──────────┼──> PN532 VCC
                                    │  5V ────────────┼──> IR Sensor A VCC
                                    │  5V ────────────┼──> IR Sensor B VCC
                                    │  GND ───────────┼──> All GND
                                    └─────────────────┘
                                             │
                                             │
         ┌───────────────────────────────────┼───────────────────────────────────────┐
         │                                   │                                       │
         │                                   │                                       │
    ┌────▼────────┐                   ┌──────▼──────┐               ┌────────────────▼──────┐
    │ SN65HVD230  │                   │   PN532     │               │  IR Sensors (Dual)    │
    │  (3.3V)     │                   │   RFID      │               │  Spaced 2-6" apart    │
    │             │                   │  Reader     │               │                       │
    │  CANH ──────┼──> CAN Bus        │             │               │  [A]───→───[B]        │
    │  CANL ──────┼──> CAN Bus        │  Antenna    │               │  Entry    Exit        │
    │             │                   │             │               │                       │
    │  CTX <──────┼─── GPIO21         │  SPI Pins   │               │  Sensor A OUT ────────┼──> GPIO12
    │  CRX ───────┼──> GPIO22         │  SS/CS      │               │  Sensor B OUT ────────┼──> GPIO13
    └─────────────┘                   └─────────────┘               └───────────────────────┘
         │                                                                   │
         │                                                                   │
         └──> To other LCC nodes                            Positioned to detect train direction
              (120Ω termination at each end)                Train passes A first (A→B) or B first (B→A)



CAN Bus Topology:
═══════════════

    [Node 1]────120Ω────[Your ESP32 Node]────[Node 3]────120Ω────[Node 4]
                             (This device)


Component Voltages:
══════════════════
✓ ESP32:        3.3V logic, 5V input power via USB/VIN
✓ SN65HVD230:   3.3V (CRITICAL - NOT 5V!)
✓ PN532:        3.3V 
✓ IR Sensor:    Usually 5V (check datasheet - some are 3.3V)
✓ CAN Bus:      Differential signaling (CANH/CANL typically 2.5V ± 1V)
```

## Connection Priority

Wire in this order for easiest troubleshooting:

1. **Power First**
   - Connect all GND
   - Connect 3.3V to SN65HVD230 and PN532
   - Connect 5V to IR sensor (if needed)
   - **Test:** Measure voltages before connecting signal wires

2. **IR Sensor** (Simplest)
   - Connect GPIO13 to sensor OUT
   - **Test:** Monitor serial output, wave hand through beam

3. **CAN Bus**
   - Connect GPIO21 → CTX, GPIO22 → CRX
   - Connect CANH/CANL to bus with 120Ω termination
   - **Test:** Check for CAN frames on bus with oscilloscope/analyzer

4. **RFID Reader**
   - Connect SPI pins (SCK, MISO, MOSI, CS)
   - Optionally connect IRQ and RESET
   - **Test:** Place RFID tag near antenna, check serial output

## Typical Breadboard Layout

```
                 ESP32 Dev Board
                 ┌─────────────┐
    Rail(+) ─────┤ VIN    3.3V ├───── Rail(+)
    Rail(-) ─────┤ GND     GND ├───── Rail(-)
                 │             │
               ──┤  4       5  ├────── CAN TX → SN65HVD230
  CAN RX ←───────┤  5          │      (Note: Some boards label differently)
                 │             │
  IR Sensor A ───┤ 12          │
  IR Sensor B ───┤ 13          │
                 │             │
PN532 IRQ ───────┤ 14          │
PN532 CS ────────┤ 15          │
PN532 RST ───────┤ 16          │
                 │             │
PN532 SCK ───────┤ 18          │
PN532 MISO ──────┤ 19          │
                 │             │
PN532 MOSI ──────┤ 23          │
                 └─────────────┘

**CRITICAL:** 
- GPIO4 = CAN RX (from SN65HVD230 CRX)
- GPIO5 = CAN TX (to SN65HVD230 CTX)

                 Place SN65HVD230, PN532, and both IR sensors
                 near ESP32 with short jumper wires
                 
                 Mount IR sensors 2-6" apart along track
```

## Direction Detection Setup

Physical sensor placement is critical for direction detection:

```
Track Direction →→→

    ╔═══════╗         ╔═══════╗
    ║   A   ║         ║   B   ║
    ║Sensor ║←─2-6"──→║Sensor ║
    ║(Entry)║         ║(Exit) ║
    ╚═══════╝         ╚═══════╝
       ↓                 ↓
    GPIO12            GPIO13

Train enters from left:
  1. Breaks Sensor A first → Start timing
  2. Breaks Sensor B within 2 sec → Direction = A→B (Forward)
  
Train enters from right:
  1. Breaks Sensor B first → Start timing
  2. Breaks Sensor A within 2 sec → Direction = B→A (Reverse)
```

## Important Notes

⚠️ **DO NOT** connect 5V to SN65HVD230 - it will be damaged!
⚠️ CAN bus MUST have 120Ω termination resistors at BOTH ends
⚠️ **CAN PINS:** GPIO5=TX, GPIO4=RX (OpenMRNLite defaults, NOT GPIO21/22!)
⚠️ PN532 DIP switches: SW1=ON, SW2=OFF for SPI mode
⚠️ Keep SPI wires short (< 6") for reliable operation
⚠️ If using long CAN bus runs, use twisted pair wire for CANH/CANL
⚠️ **IR sensor spacing**: Place sensors 2-6" apart for reliable direction detection
⚠️ **Sensor orientation**: Ensure consistent "entry" (A) and "exit" (B) positioning
✓ Test each component individually before integrating
✓ Test direction detection by moving hand/object through sensors slowly
✓ Verify direction events trigger correctly in both directions
✓ Use a multimeter to verify all connections before powering on
✓ Monitor serial output at 115200 baud for debugging
