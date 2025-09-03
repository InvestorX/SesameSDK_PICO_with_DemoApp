# SesameSDK for Raspberry Pi Pico W

This is a port of the ESP32 SesameSDK to Raspberry Pi Pico W, enabling BLE communication with Sesame5 smart locks.

## Requirements

- Raspberry Pi Pico W (required for Bluetooth functionality)
- Pico SDK installed and configured
- CMake 3.13 or later

## Building

1. Set up the Pico SDK environment:
```bash
export PICO_SDK_PATH=/path/to/pico-sdk
```

2. Create a build directory and build the project:
```bash
mkdir build
cd build
cmake ..
make
```

3. Flash the generated `SesameSDK_Pico.uf2` file to your Pico W

## Features

- **Automatic device discovery**: Automatically scans for and connects to nearby Sesame5 smart locks
- **Auto-lock functionality**: When Sesame5 reaches unlock position, Pico W will automatically send a lock command
- **BLE communication**: Uses BTstack for robust Bluetooth Low Energy communication

## Hardware Setup

1. Connect Raspberry Pi Pico W to your computer via USB
2. The device will appear as a USB storage device when in bootloader mode
3. Copy the `.uf2` file to the device
4. The Pico W will automatically restart and begin scanning for Sesame devices

## Differences from ESP32 Version

- **Build System**: Uses Pico SDK CMake instead of ESP-IDF
- **BLE Stack**: Uses BTstack instead of NimBLE
- **Logging**: Uses printf instead of ESP_LOG macros
- **Hardware**: Targets Pico W instead of ESP32-C3

## Code Structure

```
src/
├── main.c              # Main application entry point
├── ble_central.c       # BLE central implementation using BTstack
├── ble_central.h       # BLE interface definitions
├── candy.h             # Sesame protocol constants
├── sesame/
│   ├── ssm.c           # Sesame state machine
│   ├── ssm.h           # Sesame data structures
│   ├── ssm_cmd.c       # Sesame command handling
│   └── ssm_cmd.h       # Sesame command definitions
└── utils/
    ├── uECC.c          # Elliptic curve cryptography
    ├── aes-cbc-cmac.c  # AES encryption
    ├── c_ccm.c         # CCM mode encryption
    ├── TI_aes_128.c    # AES-128 implementation
    └── utils.c         # Utility functions
```

## Serial Output

The Pico W will output debug information via USB CDC. Connect to the virtual COM port to see:
- Bluetooth initialization status
- Device discovery messages
- Connection status
- Sesame lock/unlock events

## License

This project maintains the same MIT license as the original ESP32 version.

## Acknowledgments

- Original ESP32 implementation by CANDY HOUSE
- Ported to Raspberry Pi Pico W using Pico SDK and BTstack