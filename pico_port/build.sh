#!/bin/bash

# Build script for SesameSDK Pico Port
# This script builds the project and provides helpful output

set -e

echo "=========================================="
echo "SesameSDK Pico W Build Script"
echo "=========================================="

# Check if PICO_SDK_PATH is set
if [ -z "$PICO_SDK_PATH" ]; then
    echo "ERROR: PICO_SDK_PATH environment variable is not set"
    echo "Please set it to the path of your Pico SDK installation:"
    echo "  export PICO_SDK_PATH=/path/to/pico-sdk"
    exit 1
fi

echo "Using Pico SDK at: $PICO_SDK_PATH"

# Check if the SDK path exists
if [ ! -d "$PICO_SDK_PATH" ]; then
    echo "ERROR: Pico SDK path does not exist: $PICO_SDK_PATH"
    exit 1
fi

# Create build directory
echo "Creating build directory..."
mkdir -p build
cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake ..

# Build the project
echo "Building SesameSDK_Pico..."
make -j$(nproc)

echo "=========================================="
echo "Build completed successfully!"
echo ""
echo "Output files:"
echo "  - SesameSDK_Pico.uf2  (Flash this to your Pico W)"
echo "  - SesameSDK_Pico.elf  (ELF binary)"
echo "  - SesameSDK_Pico.bin  (Raw binary)"
echo ""
echo "To flash to Pico W:"
echo "1. Hold BOOTSEL button while connecting Pico W to USB"
echo "2. Copy SesameSDK_Pico.uf2 to the RPI-RP2 drive"
echo "3. The Pico W will reboot and start the Sesame SDK"
echo "=========================================="