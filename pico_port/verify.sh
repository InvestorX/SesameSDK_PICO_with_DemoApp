#!/bin/bash

# Basic syntax verification for SesameSDK Pico Port
# This checks for common C syntax issues without requiring Pico SDK

echo "=========================================="
echo "SesameSDK Pico W/Pico 2 Syntax Verification"
echo "=========================================="

# Check if gcc is available for basic syntax checking
if ! command -v gcc &> /dev/null; then
    echo "WARNING: gcc not found, skipping syntax checks"
    exit 0
fi

echo "Checking C file syntax..."

# Find all C files and check basic syntax
find src/ -name "*.c" -type f | while read -r file; do
    echo "Checking $file..."
    
    # Basic syntax check (will fail on missing headers but catch syntax errors)
    gcc -fsyntax-only -std=c11 -I src/ -I src/sesame/ -I src/utils/ \
        -D__PICO_SDK__ -D__BTSTACK__ \
        "$file" 2>/dev/null
    
    if [ $? -eq 0 ]; then
        echo "  ✓ $file - syntax OK (ignoring missing headers)"
    else
        echo "  ✗ $file - potential syntax issues"
        # Show specific errors but filter out missing header warnings
        gcc -fsyntax-only -std=c11 -I src/ -I src/sesame/ -I src/utils/ \
            -D__PICO_SDK__ -D__BTSTACK__ \
            "$file" 2>&1 | grep -v "fatal error.*No such file" | head -5
    fi
done

echo ""
echo "File structure check:"
echo "  Source files: $(find src/ -name "*.c" | wc -l)"
echo "  Header files: $(find src/ -name "*.h" | wc -l)"
echo "  Total files:  $(find src/ -type f | wc -l)"

echo ""
echo "Key files verification:"
for file in "src/main.c" "src/ble_central.c" "src/sesame/ssm.c" "CMakeLists.txt"; do
    if [ -f "$file" ]; then
        echo "  ✓ $file exists"
    else
        echo "  ✗ $file missing"
    fi
done

echo ""
echo "=========================================="
echo "Basic verification complete!"
echo ""
echo "NOTE: Full compilation requires Pico SDK installation."
echo "Run './build.sh' for Pico W or './build.sh pico2' for Pico 2."
echo "=========================================="