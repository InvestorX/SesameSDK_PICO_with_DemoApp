#include "ble_central.h"
#include "ssm.h"
#include "ssm_cmd.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "btstack.h"
#include <stdio.h>
#include <string.h>

static const char * TAG = "ble_central.c";

// External declaration for Sesame environment
extern struct ssm_env_tag * p_ssms_env;

// HCI event callback registration
static btstack_packet_callback_registration_t hci_event_callback_registration;

// Sesame5 UUIDs
static const uint8_t ssm_service_uuid[] = {0x81, 0xFD}; // 0xFD81
static const uint8_t ssm_char_uuid[] = {0x16, 0x86, 0x00, 0x02, 0xa5, 0xae, 0x98, 0x56, 0xb6, 0xd3, 0xdb, 0xb4, 0xc6, 0x76, 0x99, 0x3e};
static const uint8_t ssm_notify_uuid[] = {0x16, 0x86, 0x00, 0x03, 0xa5, 0xae, 0x98, 0x56, 0xb6, 0xd3, 0xdb, 0xb4, 0xc6, 0x76, 0x99, 0x3e};

static hci_con_handle_t connection_handle = HCI_CON_HANDLE_INVALID;
static gatt_client_service_t sesame_service;
static gatt_client_characteristic_t write_characteristic;
static gatt_client_characteristic_t notify_characteristic;
static gatt_client_characteristic_descriptor_t notify_descriptor;

static bool scanning = false;
static bool connected = false;
static bool service_discovered = false;

static void packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
static void gatt_client_event_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);

void pico_ble_gatt_write(sesame * ssm, uint8_t * value, uint16_t length) {
    if (!connected || !service_discovered) {
        printf("[ERROR] %s: Not connected or service not discovered\n", TAG);
        return;
    }
    
    gatt_client_write_value_of_characteristic(
        packet_handler, 
        connection_handle, 
        write_characteristic.value_handle, 
        length, 
        value
    );
}

static void start_scan(void) {
    if (scanning) return;
    
    printf("[INFO] %s: Starting BLE scan\n", TAG);
    scanning = true;
    gap_set_scan_parameters(0, 0x0030, 0x0030);
    gap_start_scan();
}

static void stop_scan(void) {
    if (!scanning) return;
    
    printf("[INFO] %s: Stopping BLE scan\n", TAG);
    scanning = false;
    gap_stop_scan();
}

static void connect_to_device(bd_addr_t addr, bd_addr_type_t addr_type) {
    printf("[INFO] %s: Connecting to device\n", TAG);
    stop_scan();
    gap_connect(addr, addr_type);
}

static void discover_services(void) {
    printf("[INFO] %s: Discovering services\n", TAG);
    gatt_client_discover_primary_services_by_uuid16(
        gatt_client_event_handler, 
        connection_handle, 
        (ssm_service_uuid[1] << 8) | ssm_service_uuid[0] // 0xFD81
    );
}

static void discover_characteristics(void) {
    printf("[INFO] %s: Discovering characteristics\n", TAG);
    gatt_client_discover_characteristics_for_service(
        gatt_client_event_handler,
        connection_handle,
        &sesame_service
    );
}

static void enable_notifications(void) {
    printf("[INFO] %s: Enabling notifications\n", TAG);
    gatt_client_write_client_characteristic_configuration(
        gatt_client_event_handler,
        connection_handle,
        &notify_characteristic,
        GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION
    );
}

static void packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size) {
    UNUSED(channel);
    UNUSED(size);

    if (packet_type != HCI_EVENT_PACKET) return;

    switch (hci_event_packet_get_type(packet)) {
        case BTSTACK_EVENT_STATE:
            if (btstack_event_state_get_state(packet) == HCI_STATE_WORKING) {
                printf("[INFO] %s: Bluetooth ready, starting scan\n", TAG);
                start_scan();
            }
            break;

        case GAP_EVENT_ADVERTISING_REPORT: {
            bd_addr_t address;
            gap_event_advertising_report_get_address(packet, address);
            uint8_t address_type = gap_event_advertising_report_get_address_type(packet);
            uint8_t length = gap_event_advertising_report_get_data_length(packet);
            const uint8_t * data = gap_event_advertising_report_get_data(packet);

            // Check if this is a Sesame device by looking for the service UUID
            for (int i = 0; i < length - 2; i++) {
                if (data[i] == 0x03 && data[i+1] == 0x03) { // Complete 16-bit Service UUIDs
                    if (data[i+2] == ssm_service_uuid[0] && data[i+3] == ssm_service_uuid[1]) {
                        printf("[INFO] %s: Found Sesame device\n", TAG);
                        connect_to_device(address, address_type);
                        return;
                    }
                }
            }
            break;
        }

        case HCI_EVENT_LE_META:
            switch (hci_event_le_meta_get_subevent_code(packet)) {
                case HCI_SUBEVENT_LE_CONNECTION_COMPLETE:
                    connection_handle = hci_subevent_le_connection_complete_get_connection_handle(packet);
                    printf("[INFO] %s: Connected, handle=%d\n", TAG, connection_handle);
                    connected = true;
                    discover_services();
                    break;
                default:
                    break;
            }
            break;

        case HCI_EVENT_DISCONNECTION_COMPLETE:
            printf("[INFO] %s: Disconnected\n", TAG);
            connection_handle = HCI_CON_HANDLE_INVALID;
            connected = false;
            service_discovered = false;
            start_scan();
            break;

        default:
            break;
    }
}

static void gatt_client_event_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size) {
    UNUSED(packet_type);
    UNUSED(channel);
    UNUSED(size);

    switch(hci_event_packet_get_type(packet)) {
        case GATT_EVENT_SERVICE_QUERY_RESULT:
            gatt_event_service_query_result_get_service(packet, &sesame_service);
            printf("[INFO] %s: Service found\n", TAG);
            break;

        case GATT_EVENT_QUERY_COMPLETE:
            if (!service_discovered) {
                discover_characteristics();
            } else {
                enable_notifications();
            }
            break;

        case GATT_EVENT_CHARACTERISTIC_QUERY_RESULT: {
            gatt_client_characteristic_t characteristic;
            gatt_event_characteristic_query_result_get_characteristic(packet, &characteristic);
            
            if (memcmp(characteristic.uuid128, ssm_char_uuid, 16) == 0) {
                write_characteristic = characteristic;
                printf("[INFO] %s: Write characteristic found\n", TAG);
            } else if (memcmp(characteristic.uuid128, ssm_notify_uuid, 16) == 0) {
                notify_characteristic = characteristic;
                printf("[INFO] %s: Notify characteristic found\n", TAG);
            }
            break;
        }

        case GATT_EVENT_NOTIFICATION: {
            uint16_t value_length = gatt_event_notification_get_value_length(packet);
            const uint8_t * value = gatt_event_notification_get_value(packet);
            
            printf("[INFO] %s: Received notification, length=%d\n", TAG, value_length);
            
            // Handle the received data from Sesame
            if (p_ssms_env) {
                ssm_ble_receiver(&p_ssms_env->ssm, value, value_length);
            }
            break;
        }

        case GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT:
            service_discovered = true;
            printf("[INFO] %s: Service discovery complete\n", TAG);
            
            // Initialize Sesame after service discovery
            if (p_ssms_env) {
                send_reg_cmd_to_ssm(&p_ssms_env->ssm);
            }
            break;

        default:
            break;
    }
}

void pico_ble_init(void) {
    printf("[INFO] %s: Initializing Bluetooth\n", TAG);

    // Initialize CYW43 
    if (cyw43_arch_init()) {
        printf("[ERROR] %s: Failed to initialize cyw43\n", TAG);
        return;
    }

    // Initialize BTstack
    l2cap_init();
    sm_init();
    gatt_client_init();

    hci_event_callback_registration.callback = &packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);

    // Turn on Bluetooth
    hci_power_control(HCI_POWER_ON);

    printf("[INFO] %s: Bluetooth initialization complete\n", TAG);
}