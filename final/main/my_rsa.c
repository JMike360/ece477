/* ESP32 and GUI side RSA references
https://tls.mbed.org/api/structmbedtls__rsa__context.html
https://docs.microsoft.com/en-us/dotnet/api/system.security.cryptography.rsacryptoserviceprovider?view=net-5.0
https://docs.microsoft.com/en-us/dotnet/api/system.security.cryptography.rsaparameters?view=net-5.0
*/

#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <esp_system.h>
#include "mbedtls/rsa.h"
#include "mbedtls/pk.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/entropy_poll.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "unity.h"
#include "sdkconfig.h"
#include "../include/my_rsa.h"
#include "esp_log.h"
#include "../include/bt.h"

#define TAG "RSA"

static mbedtls_rsa_context my_rsa;
static mbedtls_rsa_context client_rsa;

static int key_exchange_complete = 0;

#pragma pack(1)
typedef struct {
    uint64_t public_exp;
    char divider;
    uint64_t public_mod[RSA_KEYLEN_IN_BITS / 64];
    char end;
} rsa_pub_info;

int isKeyExchanged() {
    return key_exchange_complete;
}

void resetKeyExchange() {
    key_exchange_complete = 0;
}

static int myrand(void *rng_state, unsigned char *output, size_t len) {
    size_t olen;
    return mbedtls_hardware_poll(rng_state, output, len, &olen);
}

int my_rsa_init() {
    mbedtls_rsa_init(&my_rsa, MBEDTLS_RSA_PRIVATE, 0);
    int err = mbedtls_rsa_gen_key(&my_rsa, myrand, NULL, RSA_KEYLEN_IN_BITS, 65537);
    if (err != 0) {
        ESP_LOGE(TAG, "Failed to initialize RSA due to error: %x", err);
        return RSA_FAILURE;
    }
    mbedtls_rsa_set_padding(&my_rsa, MBEDTLS_RSA_PKCS_V15, 0);

    if ((int)my_rsa.len * 8 != RSA_KEYLEN_IN_BITS) {
        ESP_LOGE(TAG, "Incorrect N generated: Expected %d got %d", RSA_KEYLEN_IN_BITS, (int)my_rsa.len * 8);
        return RSA_FAILURE;
    }

    int rsa_priv_len = (int)my_rsa.D.n * sizeof(mbedtls_mpi_uint) * 8;
    if ((int)my_rsa.D.n * sizeof(mbedtls_mpi_uint) * 8 != RSA_KEYLEN_IN_BITS)
        ESP_LOGE(TAG, "Incorrect private exponent length generated: Expected %d got %d", RSA_KEYLEN_IN_BITS, rsa_priv_len);

    ESP_LOGI(TAG, "Successfully initialized RSA context");
    return RSA_SUCCESS;
}

int my_rsa_key_send() {
    rsa_pub_info key_to_send = {.public_exp = 0, .divider = '\n', .public_mod = {0}, .end = '\n'};
    key_to_send.public_exp = *my_rsa.E.p;
    memcpy(key_to_send.public_mod, my_rsa.N.p, my_rsa.N.n * sizeof(mbedtls_mpi_uint));
    btSendData((uint8_t*)&key_to_send, sizeof(key_to_send));
    ESP_LOGI(TAG, "Successfully sent RSA key pair");

    uint8_t* buf = (uint8_t*)(&key_to_send);
    ESP_LOGI(TAG, "-----------------------");
    for(int i = 0; i < sizeof(key_to_send); i+= 8) {
        ESP_LOGI(TAG, "%02x %02x %02x %02x %02x %02x %02x %02x", buf[i], buf[i+1], buf[i+2], buf[i+3], buf[i+4], buf[i+5], buf[i+6], buf[i+7]);
    }
    ESP_LOGI(TAG, "-----------------------");

    return RSA_SUCCESS;
}

int my_rsa_key_recv(uint8_t* data) {
    rsa_pub_info key_to_recv;
    memcpy(&key_to_recv, data, sizeof(key_to_recv));
    if (key_to_recv.divider != '\n' || key_to_recv.end != '\n'){
        ESP_LOGE(TAG, "Invalid divider characters detected");    
        return RSA_FAILURE;
    }

    client_rsa.E.n = 1;
    client_rsa.E.s = 1;
    client_rsa.E.p = calloc(client_rsa.E.n, sizeof(mbedtls_mpi_uint));
    *client_rsa.E.p = key_to_recv.public_exp;

    client_rsa.N.n = RSA_KEYLEN_IN_BYTES / sizeof(mbedtls_mpi_uint);
    client_rsa.N.s = 1;
    client_rsa.N.p = calloc(client_rsa.N.n, sizeof(mbedtls_mpi_uint));
    memcpy(client_rsa.N.p, key_to_recv.public_mod, client_rsa.N.n * sizeof(mbedtls_mpi_uint));

    client_rsa.len = RSA_KEYLEN_IN_BYTES;
    key_exchange_complete = 1;

    ESP_LOGI(TAG, "Successfully received RSA key pair");

    uint8_t* buf = (uint8_t*)(&key_to_recv);
    ESP_LOGI(TAG, "-----------------------");
    for(int i = 0; i < sizeof(key_to_recv); i+= 8) {
        ESP_LOGI(TAG, "%02x %02x %02x %02x %02x %02x %02x %02x", buf[i], buf[i+1], buf[i+2], buf[i+3], buf[i+4], buf[i+5], buf[i+6], buf[i+7]);
    }
    ESP_LOGI(TAG, "-----------------------");

    return RSA_SUCCESS;
}

int my_rsa_encrypt(uint8_t* plaintext, uint8_t** ciphertext) {
    ESP_LOGI(TAG, "Attempting to encrypted from %s", plaintext);
    if (key_exchange_complete == 0) {
        ESP_LOGE(TAG, "Failed to encrypt, client public key not received");
        return RSA_FAILURE;
    }
        
    *ciphertext = calloc(RSA_KEYLEN_IN_BYTES, sizeof(**ciphertext));
    if (mbedtls_rsa_rsaes_pkcs1_v15_encrypt(&client_rsa, myrand, NULL, MBEDTLS_RSA_PUBLIC, strlen((char*)plaintext), plaintext, *ciphertext) != 0) {
        ESP_LOGE(TAG, "Failed to encrypt from %s", plaintext);
        return RSA_FAILURE;
    }
    ESP_LOGI(TAG, "Successfully encrypted from %s", plaintext);
    return RSA_SUCCESS;
}

int my_rsa_decrypt(uint8_t* ciphertext, uint8_t** plaintext) {
    ESP_LOGI(TAG, "Attempting to decrypt");
    if (key_exchange_complete == 0) {
        ESP_LOGE(TAG, "Failed to decrypt, client public key not received");
        return RSA_FAILURE;
    }

    *plaintext = calloc(RSA_KEYLEN_IN_BYTES, sizeof(**plaintext));
    size_t outlen = 0;
    if (mbedtls_rsa_rsaes_pkcs1_v15_decrypt(&my_rsa, myrand, NULL, MBEDTLS_RSA_PRIVATE, &outlen, ciphertext, *plaintext, RSA_KEYLEN_IN_BYTES) != 0) {
        ESP_LOGE(TAG, "Failed to decrypt to %s", *plaintext);
        return RSA_FAILURE;
    }
    ESP_LOGI(TAG, "Successfully decrypted to %s", *plaintext);
    return RSA_SUCCESS;
}

int my_rsa_deinit() {
    mbedtls_rsa_free(&my_rsa);
    mbedtls_rsa_free(&client_rsa);
    ESP_LOGI(TAG, "Successfully uninitialized RSA context");
    return RSA_SUCCESS;
}