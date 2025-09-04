cmake_minimum_required(VERSION 3.13)

# Set board type - supports both Pico W and Pico 2
# You can override this by setting PICO_BOARD environment variable:
# export PICO_BOARD=pico2_w  (for Pico 2)
# export PICO_BOARD=pico_w   (for original Pico W)
if(NOT DEFINED PICO_BOARD)
    set(PICO_BOARD pico_w)
endif()

message(STATUS "Building for board: ${PICO_BOARD}")

# Validate board type
if(NOT (PICO_BOARD STREQUAL "pico_w" OR PICO_BOARD STREQUAL "pico2_w"))
    message(FATAL_ERROR "Unsupported board: ${PICO_BOARD}. Supported boards: pico_w, pico2_w")
endif()

# Include Pico SDK
include(pico_sdk_import.cmake)

project(SesameSDK_Pico C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Pico SDK init
pico_sdk_init()

# Add executable
add_executable(SesameSDK_Pico
    src/main.c
    src/ble_central.c
    src/sesame/ssm.c
    src/sesame/ssm_cmd.c
    src/utils/uECC.c
    src/utils/aes-cbc-cmac.c
    src/utils/c_ccm.c
    src/utils/TI_aes_128.c
    src/utils/utils.c
)

# Include directories
target_include_directories(SesameSDK_Pico PRIVATE
    src/
    src/sesame/
    src/utils/
)

# Link libraries
target_link_libraries(SesameSDK_Pico 
    pico_stdlib
    pico_cyw43_arch_lwip_threadsafe_background
    pico_btstack_ble
    pico_btstack_cyw43
    pico_lwip_mbedtls
    pico_mbedtls
    hardware_flash
    hardware_sync
)

# Enable USB output and disable UART output
pico_enable_stdio_usb(SesameSDK_Pico 1)
pico_enable_stdio_uart(SesameSDK_Pico 0)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(SesameSDK_Pico)