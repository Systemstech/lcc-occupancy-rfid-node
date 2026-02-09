# ESP32 LCC Occupancy + RFID Node

An advanced LCC (Layout Command Control) node for model railroads featuring dual IR beam-break sensors for direction detection and PN532 RFID reader for rolling stock identification.

## Features

- ✅ **Dual IR Occupancy Detection** - Direction-aware train detection (A→B or B→A)
- ✅ **RFID Car Identification** - PN532 reader for unique car/locomotive tracking
- ✅ **CAN Bus Integration** - SN65HVD230 transceiver using ESP32 built-in TWAI controller
- ✅ **8 LCC Event Types** - Comprehensive event reporting for automation
- ✅ **Low Latency** - Direct TWAI connection, no SPI overhead for CAN
- ✅ **Easy Configuration** - Simple pin definitions and event ID customization

## Hardware Requirements

- ESP32 development board
- SN65HVD230 CAN transceiver module
- Two IR beam-break sensors (e.g., E18-D80NK)
- PN532 RFID reader module (SPI version)
- LCC/CAN bus network
- 12V to 5V power supply (if needed)

## Quick Start

### 1. Install Dependencies

**Arduino IDE:**
- Install ESP32 board support
- Install libraries:
  - OpenMRNLite (from GitHub: bakerstu/openmrn)
  - Adafruit PN532
  - Adafruit BusIO

**PlatformIO:**
```bash
pio lib install
```

### 2. Configure Node

Edit `lcc_occupancy_rfid_node.ino`:

```cpp
// Set your unique Node ID
#define NODE_ID 0x050101013F00ULL  // CHANGE THIS!

// Customize LCC Event IDs
const uint64_t EVENT_OCCUPIED = 0x0501010100000001ULL;
const uint64_t EVENT_CLEAR = 0x0501010100000002ULL;
// ... etc
```

### 3. Wire Hardware

See [WIRING_DIAGRAM.md](WIRING_DIAGRAM.md) for complete connection details.

**Key Connections:**
- GPIO21 → SN65HVD230 TX
- GPIO22 → SN65HVD230 RX
- GPIO12 → IR Sensor A
- GPIO13 → IR Sensor B
- GPIO15 → PN532 CS (SPI)

### 4. Upload & Test

Upload to ESP32 and monitor serial output at 115200 baud.

## Documentation

- 📖 [README.md](README.md) - Complete setup guide
- 🔌 [WIRING_DIAGRAM.md](WIRING_DIAGRAM.md) - Visual wiring guide
- 🔧 [WIRING.md](WIRING.md) - Quick reference
- 🚂 [DIRECTION_DETECTION.md](DIRECTION_DETECTION.md) - How direction detection works
- 🔢 [AXLE_COUNTING.md](AXLE_COUNTING.md) - Axle counting information

## LCC Events

| Event | ID | Trigger |
|-------|----|----|
| Block Occupied | 0x...001 | Either sensor triggered |
| Block Clear | 0x...002 | Both sensors clear |
| Direction A→B | 0x...003 | Forward/Entering |
| Direction B→A | 0x...004 | Reverse/Exiting |
| Sensor A Occupied | 0x...005 | Sensor A triggered |
| Sensor A Clear | 0x...006 | Sensor A cleared |
| Sensor B Occupied | 0x...007 | Sensor B triggered |
| Sensor B Clear | 0x...008 | Sensor B cleared |
| RFID Tag | 0x...1xxxx | RFID UID detected |

## Use Cases

- 🚦 Direction-aware block signaling
- 🔀 Automatic turnout alignment
- 🏭 Automated yard sorting by car type and direction
- 📢 Platform-specific station announcements
- 🚧 Single-track collision prevention
- 📊 Train tracking and logging
- 🎵 Sound triggers based on specific cars

## Troubleshooting

### Compilation Errors
- Ensure all libraries are installed
- Update ESP32 board package to latest
- Check OpenMRNLite compatibility

### PN532 Not Found
- Verify SPI wiring (especially CS on GPIO15)
- Check DIP switches: SW1=ON, SW2=OFF
- Ensure 3.3V power is stable

### No CAN Communication
- Verify TX/RX pins (GPIO21/22)
- Check 120Ω termination on bus
- Test CANH/CANL polarity

### Direction Not Detected
- Space sensors 2-6" apart
- Test each sensor individually
- Verify GPIO12=A, GPIO13=B

## Project Structure

```
lcc_occupancy_rfid_node/
├── lcc_occupancy_rfid_node.ino  # Main code
├── platformio.ini                # PlatformIO config
├── README.md                     # Full documentation
├── WIRING_DIAGRAM.md            # Visual wiring guide
├── WIRING.md                    # Quick pin reference
├── DIRECTION_DETECTION.md       # Direction detection guide
├── AXLE_COUNTING.md            # Axle counting info
└── .gitignore                   # Git ignore rules
```

## Contributing

Contributions welcome! Please:
1. Fork the repository
2. Create a feature branch
3. Test thoroughly
4. Submit a pull request

## License

This project uses:
- OpenMRNLite (BSD/GPL)
- Adafruit libraries (BSD)

## Support

- 📧 Issues: Use GitHub Issues
- 💬 LCC Community: https://www.openlcb.org/
- 📚 OpenMRN Docs: https://github.com/bakerstu/openmrn

## Changelog

### v1.0.0 (Initial Release)
- Dual IR sensor occupancy detection
- Direction detection (A→B, B→A)
- PN532 RFID reading
- SN65HVD230 CAN bus support
- 8 LCC event types
- ESP32 TWAI integration

## Roadmap

- [ ] Add CDI configuration support
- [ ] Axle counting with reflective sensors
- [ ] WiFi configuration interface
- [ ] JMRI integration examples
- [ ] Multi-node coordination examples
- [ ] Web dashboard for monitoring

---

**Made with ❤️ for model railroaders**
