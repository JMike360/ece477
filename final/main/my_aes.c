#include "../include/my_aes.h"
#include "esp_log.h"

#define TAG "AES"

void my_aes_encrypt(uint8_t* plaintext, uint8_t* key, uint8_t** ciphertext) {
	char plaintext_buf[256] = {0};
	memcpy(plaintext_buf, plaintext, strlen((char*)plaintext));

	uint8_t iv[16];
	memcpy(iv, key, sizeof(iv));

	*ciphertext = calloc(256, sizeof(**ciphertext));

	esp_aes_context ctx;
	esp_aes_init( &ctx );
	esp_aes_setkey( &ctx, key, 256 );

	int ret = esp_aes_crypt_cbc( &ctx, ESP_AES_ENCRYPT, 256, iv, (uint8_t*)plaintext, (uint8_t*)*ciphertext );
	if (ret == 0)
		ESP_LOGI(TAG, "Successfully encrypted %s", plaintext);
	else if (ret == ERR_ESP_AES_INVALID_INPUT_LENGTH)
		ESP_LOGE(TAG, "Failed to encrypt %s due to invalid length", plaintext);

	esp_aes_free( &ctx );
}

void my_aes_decrypt(uint8_t* ciphertext, uint8_t* key, uint8_t** plaintext) {
	*plaintext = calloc(256, sizeof(**plaintext));
	
	uint8_t iv[16];
	memcpy(iv, key, sizeof(iv));

	esp_aes_context ctx;
	esp_aes_init( &ctx );
	esp_aes_setkey( &ctx, key, 256 );

	int ret = esp_aes_crypt_cbc( &ctx, ESP_AES_DECRYPT, 256, iv, (uint8_t*)ciphertext, (uint8_t*)*plaintext );
	if (ret == 0)
		ESP_LOGI(TAG, "Successfully decrypted into %s", *plaintext);
	else if (ret == ERR_ESP_AES_INVALID_INPUT_LENGTH)
		ESP_LOGE(TAG, "Failed to decrypt due to invalid length");

	esp_aes_free( &ctx );
}

int getHashedCryptoKey(uint8_t* input, size_t inputSize, uint8_t** key, int* keySize){
	unsigned char* digest = malloc(sizeof(unsigned char)*32);
	int ret = mbedtls_sha256_ret((const unsigned char *)input, inputSize, digest, 0);
	*key = (uint8_t*)digest;
	*keySize = 32;
	ESP_LOGI(TAG, "Successfully hashed crypto key");
	return ret;
}