# Raspberry Pi Pico 2 Support

This document describes the Pico 2 compatibility and enhancements for the SesameSDK.

## What is Pico 2?

The Raspberry Pi Pico 2 is the next generation of the popular Pico microcontroller, featuring:

- **RP2350 microcontroller** with dual ARM Cortex-M33 or RISC-V Hazard3 cores
- **520KB SRAM** (vs 264KB on Pico W)
- **Enhanced security** with Arm TrustZone and signed boot
- **Backward compatibility** with existing Pico W code
- **Same form factor** and pinout as Pico W

## Compatibility

The SesameSDK works seamlessly on both Pico W and Pico 2:

### ✅ Fully Supported Features
- **BLE communication** with Sesame5 devices
- **Automatic device discovery** and pairing
- **Auto-lock functionality** 
- **Crypto operations** (AES, ECC)
- **USB serial output** for debugging

### 🚀 Pico 2 Enhancements
- **Better performance**: Faster ARM Cortex-M33 cores improve BLE processing
- **More memory**: 520KB RAM provides better headroom for crypto operations
- **Improved reliability**: Enhanced hardware capabilities

## Building for Pico 2

### Quick Start
```bash
# Set up Pico SDK (must support Pico 2)
export PICO_SDK_PATH=/path/to/pico-sdk

# Build for Pico 2
./build.sh pico2
```

### Manual Build
```bash
# Set board target for Pico 2
export PICO_BOARD=pico2_w

# Create build directory
mkdir build && cd build

# Configure and build
cmake ..
make
```

## Pico SDK Requirements

Ensure your Pico SDK version supports Pico 2:

- **Minimum version**: Pico SDK 2.0.0 or later
- **Recommended**: Latest stable release

Check your SDK version:
```bash
cd $PICO_SDK_PATH
git describe --tags
```

Update if needed:
```bash
cd $PICO_SDK_PATH
git pull origin master
git submodule update --init --recursive
```

## Performance Comparison

| Feature | Pico W | Pico 2 | Improvement |
|---------|--------|--------|-------------|
| **CPU Frequency** | 133 MHz | 150 MHz | +13% |
| **SRAM** | 264KB | 520KB | +97% |
| **Crypto Performance** | Good | Better | Faster AES/ECC |
| **BLE Throughput** | Good | Better | More processing headroom |

## Technical Details

### Board Configuration
- **Pico W**: `PICO_BOARD=pico_w`
- **Pico 2**: `PICO_BOARD=pico2_w`

### Memory Layout
Both boards use the same external flash (2MB minimum), but Pico 2 provides:
- More space for larger crypto buffers
- Better multitasking capabilities
- Reduced memory pressure during BLE operations

### Code Compatibility
No source code changes required - the Pico SDK handles all hardware differences transparently.

## Troubleshooting

### Build Issues
```bash
# Ensure PICO_BOARD is set correctly
echo $PICO_BOARD

# Should output: pico2_w (for Pico 2) or pico_w (for Pico W)
```

### Flash Issues
1. **Wrong board type**: Make sure you built for the correct target
2. **SDK version**: Verify Pico SDK supports Pico 2 (v2.0.0+)
3. **Hardware check**: Confirm you have a Pico 2 and not original Pico

### Runtime Issues
Both boards should behave identically - if you see differences:
1. Check serial output for error messages
2. Verify BLE functionality with known working Sesame device
3. Compare behavior between boards using same build

## Migration from Pico W

### Hardware Migration
1. **No changes needed**: Same pinout and form factor
2. **Drop-in replacement**: Use same connections and circuits
3. **Flash new firmware**: Build with `pico2` target and flash

### Software Migration
1. **Rebuild**: Use `./build.sh pico2` instead of `./build.sh`
2. **Test**: Verify functionality with your Sesame devices
3. **Enjoy**: Better performance with same functionality

## Future Enhancements

Potential Pico 2-specific improvements:
- **Multi-core BLE**: Utilize second core for BLE processing
- **Enhanced crypto**: Take advantage of hardware security features
- **Power management**: Use advanced sleep modes
- **Concurrent operations**: Handle multiple Sesame devices simultaneously

## Support

For Pico 2-specific issues:
1. Check this documentation first
2. Verify Pico SDK version compatibility
3. Compare behavior with Pico W build
4. Report issues with hardware details and SDK version