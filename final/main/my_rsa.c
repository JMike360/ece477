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

static int client_rsa_received = 0;

typedef struct {
    uint64_t public_exp;
    char divider;
    uint64_t public_mod[64];
    char end;
} rsa_pub_info;

static int myrand(void *rng_state, unsigned char *output, size_t len) {
    size_t olen;
    return mbedtls_hardware_poll(rng_state, output, len, &olen);
}

void mbedtls_mpi_printf(const char *name, const mbedtls_mpi *X)
{
    static char buf[2048];
    size_t n;
    memset(buf, 0, sizeof(buf));
    mbedtls_mpi_write_string(X, 16, buf, sizeof(buf)-1, &n);
    if(n) {
        printf("%s = (s=%d) 0x%s\n", name, X->s, buf);
    } else {
        printf("%s = TOOLONG\n", name);
    }
}

int my_rsa_init() {
    mbedtls_rsa_init(&my_rsa, MBEDTLS_RSA_PRIVATE, 0);
    int err = mbedtls_rsa_gen_key(&my_rsa, myrand, NULL, KEYSIZE, 65537);
    if (err != 0) {
        ESP_LOGE(TAG, "Failed to initialize RSA due to error: %x", err);
        return RSA_FAILURE;
    }

    if ((int)my_rsa.len * 8 != KEYSIZE) {
        ESP_LOGE(TAG, "Incorrect N generated: Expected %d got %d", KEYSIZE, (int)my_rsa.len * 8);
        return RSA_FAILURE;
    }

    int rsa_priv_len = (int)my_rsa.D.n * sizeof(mbedtls_mpi_uint) * 8;
    if ((int)my_rsa.D.n * sizeof(mbedtls_mpi_uint) * 8 != KEYSIZE)
        ESP_LOGE(TAG, "Incorrect private exponent length generated: Expected %d got %d", KEYSIZE, rsa_priv_len);

    ESP_LOGI(TAG, "Successfully initialized RSA context");
    return RSA_SUCCESS;
}

int my_rsa_key_send() {
    rsa_pub_info key_to_send = {.divider = '\n', .end = '\n'};
    key_to_send.public_exp = *my_rsa.E.p;
    memcpy(key_to_send.public_mod, my_rsa.N.p, 64 * sizeof(mbedtls_mpi_uint));
    btSendData((uint8_t*)&key_to_send, 0, sizeof(key_to_send));
    ESP_LOGI(TAG, "Successfully sent RSA key pair");
    return RSA_SUCCESS;
}

int my_rsa_key_recv(uint8_t* data) {
    rsa_pub_info key_to_recv;
    memcpy(&key_to_recv, data, sizeof(key_to_recv));

    client_rsa.E.n = 1;
    client_rsa.E.s = 1;
    client_rsa.E.p = calloc(client_rsa.E.n, sizeof(*(client_rsa.E.p)));
    *client_rsa.E.p = key_to_recv.public_exp;

    client_rsa.N.n = KEYSIZE / 32;
    client_rsa.N.s = 1;
    client_rsa.N.p = calloc(client_rsa.N.n, sizeof(*(client_rsa.N.p)));
    memcpy(client_rsa.N.p, key_to_recv.public_mod, 64 * sizeof(mbedtls_mpi_uint));

    client_rsa.len = KEYSIZE / 8;
    client_rsa_received = 1;

    ESP_LOGI(TAG, "Successfully received RSA key pair");
    return RSA_SUCCESS;
}

int my_rsa_encrypt(uint8_t* plaintext, uint8_t** ciphertext) {
    if (client_rsa_received == 0)
        return RSA_FAILURE;
        
    *ciphertext = calloc(KEYSIZE / 8, sizeof(**ciphertext));
    if (mbedtls_rsa_public(&client_rsa, plaintext, *ciphertext) != 0) {
        ESP_LOGE(TAG, "Failed to encrypt from %s", plaintext);
        return RSA_FAILURE;
    }
    ESP_LOGI(TAG, "Successfully encrypted from %s", plaintext);
    return RSA_SUCCESS;
}

int my_rsa_decrypt(uint8_t* ciphertext, uint8_t** plaintext) {
    *plaintext = calloc(KEYSIZE / 8, sizeof(**plaintext));
    if (mbedtls_rsa_private(&my_rsa, NULL, NULL, ciphertext, *plaintext) != 0) {
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