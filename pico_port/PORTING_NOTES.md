# Comparison: ESP32 vs Pico W/Pico 2 SesameSDK

## Architecture Differences

| Feature | ESP32 Version | Pico W/Pico 2 Version |
|---------|---------------|------------------------|
| **Microcontroller** | ESP32-C3-DevKitM-1 | Raspberry Pi Pico W / Pico 2 |
| **Chip** | ESP32-C3 (RISC-V) | RP2040 (ARM Cortex-M0+) / RP2350 (ARM Cortex-M33/RISC-V) |
| **SDK/Framework** | ESP-IDF | Pico SDK |
| **BLE Stack** | NimBLE | BTstack |
| **Build System** | ESP-IDF CMake | Pico SDK CMake |
| **Logging** | ESP_LOG macros | printf() |
| **Memory Management** | ESP-IDF heap | Pico SDK malloc |
| **RTOS** | FreeRTOS | Optional (bare metal by default) |

## Code Changes Summary

### Main Application (`main.c`)
- **ESP32**: Uses `nvs_flash_init()`, `esp_ble_init()`
- **Pico**: Uses `stdio_init_all()`, `pico_ble_init()`

### BLE Implementation
- **ESP32**: NimBLE with `ble_gap_*`, `ble_gattc_*` functions
- **Pico**: BTstack with `gap_*`, `gatt_client_*` functions

### Logging
- **ESP32**: `ESP_LOGI(TAG, "message")`
- **Pico**: `printf("[INFO] %s: message\n", TAG)`

### Random Number Generation
- **ESP32**: `esp_fill_random()`
- **Pico**: `rand()` (should be replaced with hardware RNG in production)

### Memory and Threading
- **ESP32**: FreeRTOS tasks and heap
- **Pico**: Single thread with `sleep_ms()` for timing

## Performance Considerations

### ESP32-C3 Advantages:
- Dedicated WiFi/BLE coprocessor
- More RAM (400KB)
- Hardware crypto acceleration
- Mature ESP-IDF ecosystem

### Pico W Advantages:
- Lower cost
- Simpler development environment
- USB-native (easier debugging)
- GPIO-rich (26 pins vs 22 on ESP32-C3)

### Pico 2 Advantages:
- Higher performance (RP2350 vs RP2040)
- More RAM (520KB vs 264KB on Pico W)
- Dual-core ARM Cortex-M33 or RISC-V Hazard3
- Enhanced security features
- Backward compatible with Pico W code

## Crypto Implementation
Both versions use the same crypto libraries:
- **uECC**: Elliptic Curve Cryptography
- **AES-CBC-CMAC**: AES encryption
- **CCM**: Counter with CBC-MAC mode

## Migration Notes

When porting from ESP32 to Pico W/Pico 2:

1. **Replace ESP-IDF includes** with Pico SDK equivalents
2. **Convert ESP logging** to printf statements  
3. **Adapt BLE stack calls** from NimBLE to BTstack
4. **Update CMakeLists.txt** for Pico SDK
5. **Handle timing differently** (no FreeRTOS tasks by default)

## Pico 2 Compatibility

The code is fully compatible with both Pico W and Pico 2:
- **Board selection**: Use `PICO_BOARD=pico2_w` for Pico 2 builds
- **No code changes needed**: Pico SDK handles hardware abstraction
- **Performance improvement**: Pico 2's faster processor benefits BLE processing
- **Memory benefits**: Extra RAM in Pico 2 provides more headroom

## Future Improvements

1. **Hardware RNG**: Replace `rand()` with `rng_get()` for better security
2. **Power Management**: Add sleep modes for battery operation
3. **Multi-core**: Utilize second Pico core for BLE processing
4. **Flash Storage**: Add NVS equivalent for persistent storage