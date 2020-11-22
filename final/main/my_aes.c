#include "../include/my_aes.h"
#include "esp_log.h"

#define TAG "AES"

void my_aes_encrypt(uint8_t* plaintext, uint8_t* key, uint8_t* ciphertext) {
	uint8_t iv[16];
	memset(iv, 0, sizeof(iv));

	esp_aes_context ctx;
	esp_aes_init( &ctx );
	esp_aes_setkey( &ctx, key, 256 );

	esp_aes_crypt_cbc( &ctx, ESP_AES_ENCRYPT, sizeof(plaintext), iv, (uint8_t*)plaintext, (uint8_t*)ciphertext );
	
	esp_aes_free( &ctx );

	ESP_LOGI(TAG, "Successfully encrypted %s into %s", plaintext, ciphertext);
}

void my_aes_decrypt(uint8_t* ciphertext, uint8_t* key, uint8_t* plaintext) {
	uint8_t iv[16];
	memcpy(iv, key, sizeof(iv));

	esp_aes_context ctx;
	esp_aes_init( &ctx );
	esp_aes_setkey( &ctx, key, 256 );

	esp_aes_crypt_cbc( &ctx, ESP_AES_DECRYPT, sizeof(ciphertext), iv, (uint8_t*)ciphertext, (uint8_t*)plaintext );

	esp_aes_free( &ctx );

	ESP_LOGI(TAG, "Successfully decrypted %s into %s", ciphertext, plaintext);
}

int getHashedCryptoKey(uint8_t* input, size_t inputSize, uint8_t** key, int* keySize){
	unsigned char* digest = malloc(sizeof(unsigned char)*32);
	int ret = mbedtls_sha256_ret((const unsigned char *)input, inputSize, digest, 0);
	*key = (uint8_t*)digest;
	*keySize = 32;
	ESP_LOGI(TAG, "Successfully hashed crypto key");
	return ret;
}