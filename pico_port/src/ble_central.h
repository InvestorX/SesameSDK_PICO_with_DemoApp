#ifndef __BLE_CENTRAL_H__
#define __BLE_CENTRAL_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "ssm.h"

void pico_ble_gatt_write(sesame * ssm, uint8_t * value, uint16_t length);

void pico_ble_init(void);

#ifdef __cplusplus
}
#endif

#endif // __BLE_CENTRAL_H__