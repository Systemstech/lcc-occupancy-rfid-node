# Netlist for Fritzing Import
## ESP32 LCC Occupancy + RFID Node

This netlist describes all electrical connections in the circuit.
You can use this to manually wire components in Fritzing or import it.

---

## Netlist Format

Format: `NET_NAME: COMPONENT.PIN, COMPONENT.PIN, ...`

---

## Power Nets

```
NET_3V3:
  ESP32.3V3
  SN65HVD230.VCC
  PN532.VCC

NET_5V:
  ESP32.5V
  IR_EMITTER_A.VCC
  IR_EMITTER_B.VCC
  IR_RECEIVER_A.VCC
  IR_RECEIVER_B.VCC

NET_GND:
  ESP32.GND
  SN65HVD230.GND
  PN532.GND
  IR_EMITTER_A.GND
  IR_EMITTER_B.GND
  IR_RECEIVER_A.GND
  IR_RECEIVER_B.GND
  CAN_TERMINATION.1
```

---

## CAN Bus Nets

```
NET_CAN_TX:
  ESP32.GPIO5
  SN65HVD230.CTX

NET_CAN_RX:
  ESP32.GPIO4
  SN65HVD230.CRX

NET_CANH:
  SN65HVD230.CANH
  CAN_BUS_CONNECTOR.CANH
  CAN_TERMINATION.2

NET_CANL:
  SN65HVD230.CANL
  CAN_BUS_CONNECTOR.CANL
  CAN_TERMINATION.3
```

---

## SPI Bus Nets (PN532 RFID)

```
NET_SPI_SCK:
  ESP32.GPIO18
  PN532.SCK

NET_SPI_MISO:
  ESP32.GPIO19
  PN532.MISO

NET_SPI_MOSI:
  ESP32.GPIO23
  PN532.MOSI

NET_SPI_CS:
  ESP32.GPIO15
  PN532.SS

NET_RFID_RST:
  ESP32.GPIO16
  PN532.RSTPDN

NET_RFID_IRQ:
  ESP32.GPIO14
  PN532.IRQ
```

---

## IR Sensor Nets

```
NET_IR_SENSOR_A:
  ESP32.GPIO12
  IR_RECEIVER_A.SIGNAL

NET_IR_SENSOR_B:
  ESP32.GPIO13
  IR_RECEIVER_B.SIGNAL
```

---

## Component List with Pin Assignments

### ESP32 DevKit V1
```
Component: ESP32
Pins:
  GPIO4  → NET_CAN_RX
  GPIO5  → NET_CAN_TX
  GPIO12 → NET_IR_SENSOR_A
  GPIO13 → NET_IR_SENSOR_B
  GPIO14 → NET_RFID_IRQ
  GPIO15 → NET_SPI_CS
  GPIO16 → NET_RFID_RST
  GPIO18 → NET_SPI_SCK
  GPIO19 → NET_SPI_MISO
  GPIO23 → NET_SPI_MOSI
  3V3    → NET_3V3
  5V     → NET_5V
  GND    → NET_GND
```

### SN65HVD230 CAN Transceiver
```
Component: SN65HVD230
Pins:
  CTX  → NET_CAN_TX
  CRX  → NET_CAN_RX
  VCC  → NET_3V3
  GND  → NET_GND
  CANH → NET_CANH
  CANL → NET_CANL
```

### PN532 RFID Module
```
Component: PN532
Pins:
  SCK    → NET_SPI_SCK
  MISO   → NET_SPI_MISO
  MOSI   → NET_SPI_MOSI
  SS     → NET_SPI_CS
  RSTPDN → NET_RFID_RST
  IRQ    → NET_RFID_IRQ
  VCC    → NET_3V3
  GND    → NET_GND
```

### IR Emitter A
```
Component: IR_EMITTER_A
Pins:
  VCC (Red)   → NET_5V
  GND (Black) → NET_GND
```

### IR Receiver A
```
Component: IR_RECEIVER_A
Pins:
  VCC (Red)       → NET_5V
  GND (Black)     → NET_GND
  SIGNAL (Yellow) → NET_IR_SENSOR_A
```

### IR Emitter B
```
Component: IR_EMITTER_B
Pins:
  VCC (Red)   → NET_5V
  GND (Black) → NET_GND
```

### IR Receiver B
```
Component: IR_RECEIVER_B
Pins:
  VCC (Red)      → NET_5V
  GND (Black)    → NET_GND
  SIGNAL (White) → NET_IR_SENSOR_B
```

### CAN Termination Resistor (120Ω)
```
Component: CAN_TERMINATION (120Ω)
Pins:
  1 → NET_GND
  2 → NET_CANH
  3 → NET_CANL
```

### CAN Bus Connector
```
Component: CAN_BUS_CONNECTOR (Terminal Block)
Pins:
  CANH → NET_CANH
  CANL → NET_CANL
```

---

## Fritzing Import Instructions

### Method 1: Manual Entry
1. Open Fritzing
2. Add all components from the Component List
3. For each NET in the netlist:
   - Click first component/pin
   - Drag wire to second component/pin
   - Repeat for all connections in that net
4. Use color coding from wire recommendations below

### Method 2: Semi-Automated (Advanced)
1. Save this netlist as `lcc_node.net`
2. In Fritzing, go to File → Import
3. Select the netlist file
4. Fritzing will auto-place components
5. Use auto-route or manually clean up wiring
6. Verify all connections against netlist

---

## Recommended Wire Colors for Fritzing

```
NET_3V3         → Red (thin or with white stripe)
NET_5V          → Red (thick or solid red)
NET_GND         → Black
NET_CAN_TX      → Orange
NET_CAN_RX      → Yellow
NET_CANH        → Green
NET_CANL        → Blue
NET_SPI_SCK     → Blue
NET_SPI_MISO    → Green
NET_SPI_MOSI    → Purple
NET_SPI_CS      → Gray
NET_RFID_RST    → White
NET_RFID_IRQ    → Brown
NET_IR_SENSOR_A → Yellow
NET_IR_SENSOR_B → White
```

---

## Connection Summary Table

| From Component | Pin | To Component | Pin | Wire Color | Notes |
|----------------|-----|--------------|-----|------------|-------|
| ESP32 | GPIO5 | SN65HVD230 | CTX | Orange | CAN TX |
| ESP32 | GPIO4 | SN65HVD230 | CRX | Yellow | CAN RX |
| ESP32 | GPIO12 | IR_RECEIVER_A | SIGNAL | Yellow | Sensor A |
| ESP32 | GPIO13 | IR_RECEIVER_B | SIGNAL | White | Sensor B |
| ESP32 | GPIO15 | PN532 | SS | Gray | SPI CS |
| ESP32 | GPIO18 | PN532 | SCK | Blue | SPI Clock |
| ESP32 | GPIO19 | PN532 | MISO | Green | SPI MISO |
| ESP32 | GPIO23 | PN532 | MOSI | Purple | SPI MOSI |
| ESP32 | GPIO16 | PN532 | RSTPDN | White | RFID Reset |
| ESP32 | GPIO14 | PN532 | IRQ | Brown | RFID IRQ |
| ESP32 | 3.3V | SN65HVD230 | VCC | Red | Power |
| ESP32 | 3.3V | PN532 | VCC | Red | Power |
| ESP32 | 5V | IR_EMITTER_A | VCC | Red | Power |
| ESP32 | 5V | IR_EMITTER_B | VCC | Red | Power |
| ESP32 | 5V | IR_RECEIVER_A | VCC | Red | Power |
| ESP32 | 5V | IR_RECEIVER_B | VCC | Red | Power |
| ESP32 | GND | All Components | GND | Black | Ground |
| SN65HVD230 | CANH | CAN_BUS | CANH | Green | CAN High |
| SN65HVD230 | CANL | CAN_BUS | CANL | Blue | CAN Low |
| CAN_BUS | CANH | TERMINATION | Pin2 | Green | 120Ω |
| CAN_BUS | CANL | TERMINATION | Pin3 | Blue | 120Ω |

---

## Verification Checklist

After wiring in Fritzing, verify:

- [ ] All ESP32 GPIO pins match netlist
- [ ] SN65HVD230 connected to 3.3V (NOT 5V!)
- [ ] PN532 connected to 3.3V (NOT 5V!)
- [ ] IR sensors connected to 5V
- [ ] All grounds connected together
- [ ] CAN termination resistor (120Ω) in place
- [ ] PN532 SPI pins correct (CS on GPIO15)
- [ ] CAN TX on GPIO5, RX on GPIO4
- [ ] IR Sensor A on GPIO12
- [ ] IR Sensor B on GPIO13
- [ ] No shorts between power rails
- [ ] All connections match wire color guide

---

## Export to Other Formats

This netlist can be converted to:
- **KiCad netlist** (.net)
- **SPICE netlist** (.cir)
- **Eagle netlist** (.sch)
- **Gerber files** (for PCB manufacturing)

---

## Notes

1. This netlist represents the ELECTRICAL connections only
2. Physical placement/routing is up to you in Fritzing
3. Component orientations matter - check datasheets
4. Some components may need pull-up/pull-down resistors (already handled in code via INPUT_PULLUP)
5. Power supply should provide at least 500mA at 5V

---

Ready to import into Fritzing!
