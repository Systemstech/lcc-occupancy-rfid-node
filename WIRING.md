# Quick Wiring Reference

## Complete Pin Connections

### ESP32 Pin Assignment Summary
```
GPIO5  --> SN65HVD230 CTX (CAN TX)
GPIO4  --> SN65HVD230 CRX (CAN RX)
GPIO18 --> PN532 SCK (SPI Clock)
GPIO19 --> PN532 MISO (SPI Data In)
GPIO23 --> PN532 MOSI (SPI Data Out)
GPIO15 --> PN532 SS/CS (Chip Select)
GPIO16 --> PN532 RESET (optional)
GPIO14 --> PN532 IRQ (optional)
GPIO12 --> IR Beam-Break Sensor A (Entry)
GPIO13 --> IR Beam-Break Sensor B (Exit)
```

**Important:** OpenMRNLite for ESP32 uses GPIO5 and GPIO4 as the default CAN pins.

### ESP32 Built-in CAN Controller (TWAI)
The ESP32 has a built-in CAN controller called TWAI (Two-Wire Automotive Interface).
It connects directly to the SN65HVD230 transceiver:
- **CTX** (GPIO21) - CAN Transmit
- **CRX** (GPIO22) - CAN Receive

No SPI interface needed for CAN - it's a direct UART-like connection!

### PN532 SPI Connection
The PN532 RFID reader uses the ESP32's SPI bus:
- **SCK** (GPIO18) - Clock signal
- **MISO** (GPIO19) - Master In Slave Out
- **MOSI** (GPIO23) - Master Out Slave In
- **SS/CS** (GPIO15) - Chip Select

### PN532 DIP Switch Settings
For **SPI mode**:
```
SW1: ON
SW2: OFF
```

### Power Connections
```
ESP32 3.3V --> SN65HVD230 VCC
ESP32 3.3V --> PN532 VCC
ESP32 5V   --> IR Sensor A VCC (check your sensor specs - some need 5V, some 3.3V)
ESP32 5V   --> IR Sensor B VCC (check your sensor specs - some need 5V, some 3.3V)
ESP32 GND  --> All GND (PN532, SN65HVD230, IR Sensors, CAN Bus)
```

**Important:** The SN65HVD230 requires 3.3V (NOT 5V). The ESP32 outputs 3.3V logic levels which are compatible.

### CAN Bus Termination
Add 120Ω resistors between CANH and CANL at both ends of the bus:
```
[Node1]---120Ω---[CAN Bus]---120Ω---[Node2]
```

## Troubleshooting Issues

### CAN Bus Not Working
1. **Verify wiring** - TX/RX might be swapped
2. **Check termination** - 120Ω resistors at both ends of bus
3. **Measure voltage** - Should see ~2.5V differential between CANH/CANL
4. **Test GPIO pins** - Some ESP32 boards have restrictions on certain pins
5. **Check power** - SN65HVD230 must have stable 3.3V
6. **Verify transceiver** - Measure voltage on CTX/CRX pins during transmission

### PN532 Not Responding
1. **Verify CS pin** - Must be on GPIO15 or update code
2. **Check SPI speed** - PN532 max is typically 5MHz
3. **DIP switches** - Must be SW1=ON, SW2=OFF for SPI mode
4. **Power supply** - Ensure stable 3.3V with adequate current
5. **Signal integrity** - Keep SPI wires short (< 6 inches if possible)

## Testing Individual Components

### Test SN65HVD230 CAN Only
Comment out PN532 initialization:
```cpp
// initRFID();  // Comment this line
```
Monitor serial output for CAN initialization messages.

### Test PN532 Only
Comment out CAN initialization:
```cpp
// esp_rom_gpio_connect_out_signal(CAN_TX_PIN, TWAI_TX_IDX, false, false);
// esp_rom_gpio_connect_in_signal(CAN_RX_PIN, TWAI_RX_IDX, false);
// stack.add_can_port_select("/dev/twai/twai0");  // Comment these lines
```
Try reading RFID tags and check serial output.

This helps isolate which component might have wiring issues.

## Advantages of SN65HVD230 vs MCP2515

The SN65HVD230 is simpler and more efficient:
- **No SPI needed** - Direct connection to ESP32 TWAI controller
- **Fewer pins** - Only TX, RX, and power (vs 7 pins for MCP2515)
- **Lower latency** - No SPI overhead
- **3.3V native** - No level shifting needed
- **Lower cost** - Typically cheaper than MCP2515 modules
- **Frees up SPI** - PN532 can use SPI without conflicts
