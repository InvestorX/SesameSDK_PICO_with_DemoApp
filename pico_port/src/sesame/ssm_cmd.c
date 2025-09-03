#include "ssm_cmd.h"
#include "aes-cbc-cmac.h"
#include "pico/stdlib.h"
#include "pico/rand.h"
#include "uECC.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static const char * TAG = "ssm_cmd.c";
static uint8_t tag_pico[] = { 'S', 'E', 'S', 'A', 'M', 'E', ' ', 'P', 'I', 'C', 'O', ' ' };
static uint8_t ecc_private_pico[32];

static int crypto_backend_micro_ecc_rng_callback(uint8_t * dest, unsigned size) {
    // Simple pseudo-random number generation for now
    // In a production system, you would want to use hardware RNG
    for (unsigned i = 0; i < size; i++) {
        dest[i] = (uint8_t)(rand() & 0xFF);
    }
    return 1;
}

void send_reg_cmd_to_ssm(sesame * ssm) {
    printf("[WARN] %s: " "[esp32->ssm][register]" "\n", TAG);
    uECC_set_rng(crypto_backend_micro_ecc_rng_callback);
    uint8_t ecc_public_pico[64];
    uECC_make_key_lit(ecc_public_pico, ecc_private_pico, uECC_secp256r1());
    ssm->c_offset = sizeof(ecc_public_pico) + 1;
    ssm->b_buf[0] = SSM_ITEM_CODE_REGISTRATION;
    memcpy(ssm->b_buf + 1, ecc_public_pico, sizeof(ecc_public_pico));
    talk_to_ssm(ssm, SSM_SEG_PARSING_TYPE_PLAINTEXT);
}

void handle_reg_data_from_ssm(sesame * ssm) {
    printf("[WARN] %s: " "[esp32<-ssm][register]" "\n", TAG);
    memcpy(ssm->public_key, &ssm->b_buf[13], 64);
    uint8_t ecdh_secret_ssm[32];
    uECC_shared_secret_lit(ssm->public_key, ecc_private_pico, ecdh_secret_ssm, uECC_secp256r1());
    memcpy(ssm->device_secret, ecdh_secret_ssm, 16);
    // ESP_LOG_BUFFER_HEX("deviceSecret", ssm->device_secret, 16);
    AES_CMAC(ssm->device_secret, (const unsigned char *) ssm->cipher.decrypt.random_code, 4, ssm->cipher.token);
    ssm->device_status = SSM_LOGGIN;
    p_ssms_env->ssm_cb__(ssm); // callback: ssm_action_handle() in main.c
}

void send_login_cmd_to_ssm(sesame * ssm) {
    printf("[WARN] %s: " "[esp32->ssm][login]" "\n", TAG);
    ssm->b_buf[0] = SSM_ITEM_CODE_LOGIN;
    AES_CMAC(ssm->device_secret, (const unsigned char *) ssm->cipher.decrypt.random_code, 4, ssm->cipher.token);
    memcpy(&ssm->b_buf[1], ssm->cipher.token, 4);
    ssm->c_offset = 5;
    talk_to_ssm(ssm, SSM_SEG_PARSING_TYPE_PLAINTEXT);
}

void send_read_history_cmd_to_ssm(sesame * ssm) {
    printf("[INFO] %s: " "[send_read_history_cmd_to_ssm]" "\n", TAG);
    ssm->c_offset = 2;
    ssm->b_buf[0] = SSM_ITEM_CODE_HISTORY;
    ssm->b_buf[1] = 1;
    talk_to_ssm(ssm, SSM_SEG_PARSING_TYPE_CIPHERTEXT);
}

void ssm_lock(uint8_t * tag, uint8_t tag_length) {
    // printf("[INFO] %s: " "[ssm][ssm_lock][%s]", SSM_STATUS_STR(p_ssms_env->ssm.device_status) "\n", TAG);
    sesame * ssm = &p_ssms_env->ssm;
    if (ssm->device_status >= SSM_LOGGIN) {
        if (tag_length == 0) {
            tag = tag_pico;
            tag_length = sizeof(tag_pico);
        }
        ssm->b_buf[0] = SSM_ITEM_CODE_LOCK;
        ssm->b_buf[1] = tag_length;
        ssm->c_offset = tag_length + 2;
        memcpy(ssm->b_buf + 2, tag, tag_length);
        talk_to_ssm(ssm, SSM_SEG_PARSING_TYPE_CIPHERTEXT);
    }
}
