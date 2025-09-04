#!/bin/bash

# Build script for SesameSDK Pico Port
# This script builds the project and provides helpful output
# Supports both Raspberry Pi Pico W and Pico 2

set -e

# Parse command line arguments
BOARD_TYPE="pico_w"
if [ "$1" = "pico2" ] || [ "$1" = "pico2_w" ]; then
    BOARD_TYPE="pico2_w"
    echo "=========================================="
    echo "SesameSDK Pico 2 Build Script"
    echo "=========================================="
elif [ "$1" = "pico" ] || [ "$1" = "pico_w" ] || [ -z "$1" ]; then
    BOARD_TYPE="pico_w"
    echo "=========================================="
    echo "SesameSDK Pico W Build Script"
    echo "=========================================="
else
    echo "Usage: $0 [pico|pico2]"
    echo "  pico   - Build for Raspberry Pi Pico W (default)"
    echo "  pico2  - Build for Raspberry Pi Pico 2"
    exit 1
fi

echo "Building for board: $BOARD_TYPE"

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
echo "Configuring with CMake for $BOARD_TYPE..."
export PICO_BOARD=$BOARD_TYPE
cmake ..

# Build the project
echo "Building SesameSDK_Pico..."
make -j$(nproc)

echo "=========================================="
echo "Build completed successfully!"
echo ""
echo "Output files:"
echo "  - SesameSDK_Pico.uf2  (Flash this to your $BOARD_TYPE)"
echo "  - SesameSDK_Pico.elf  (ELF binary)"
echo "  - SesameSDK_Pico.bin  (Raw binary)"
echo ""
if [ "$BOARD_TYPE" = "pico2_w" ]; then
    echo "To flash to Pico 2:"
    echo "1. Hold BOOTSEL button while connecting Pico 2 to USB"
    echo "2. Copy SesameSDK_Pico.uf2 to the RPI-RP2 drive"
    echo "3. The Pico 2 will reboot and start the Sesame SDK"
else
    echo "To flash to Pico W:"
    echo "1. Hold BOOTSEL button while connecting Pico W to USB"
    echo "2. Copy SesameSDK_Pico.uf2 to the RPI-RP2 drive"
    echo "3. The Pico W will reboot and start the Sesame SDK"
fi
echo "=========================================="