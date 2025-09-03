#include "ble_central.h"
#include "ssm_cmd.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include <stdio.h>

#define STRINGIFY(x) #x

static const char * TAG = "main.c";

static void ssm_action_handle(sesame * ssm) {
    printf("[INFO] %s: SSM status: %s\n", TAG, SSM_STATUS_STR(ssm->device_status));
    if (ssm->device_status == SSM_UNLOCKED) {
        ssm_lock(NULL, 0);
    }
}

int main(void) {
    printf("SesameSDK_Pico starting...\n");
    
    // Initialize stdio
    stdio_init_all();
    
    // Wait a bit for USB CDC to be ready
    sleep_ms(2000);
    
    printf("[INFO] %s: SesameSDK_Pico [Pico W/Pico 2 Port v1.1]\n", TAG);
    
#ifdef PICO_BOARD
    printf("[INFO] %s: Target board: %s\n", TAG, STRINGIFY(PICO_BOARD));
#endif
    
    // Initialize Sesame
    ssm_init(ssm_action_handle);
    
    // Initialize Bluetooth
    pico_ble_init();
    
    // Main loop
    while (true) {
        // Process any pending work
        sleep_ms(100);
    }
    
    return 0;
}