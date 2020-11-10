#include "../include/crypto.h"

void aes_encrypt(uint8_t* plaintext, uint8_t* key, uint8_t* ciphertext) {
	uint8_t iv[16];
	memset( iv, 0, sizeof( iv ) );

	esp_aes_context ctx;
	esp_aes_init( &ctx );
	esp_aes_setkey( &ctx, key, 256 );

	esp_aes_crypt_cbc( &ctx, ESP_AES_ENCRYPT, sizeof(plaintext), iv, (uint8_t*)plaintext, (uint8_t*)ciphertext );
	
	esp_aes_free( &ctx );
}

void aes_decrypt(uint8_t* ciphertext, uint8_t* key, uint8_t* plaintext) {
	uint8_t iv[16];
	memset( iv, 0, sizeof( iv ) );

	esp_aes_context ctx;
	esp_aes_init( &ctx );
	esp_aes_setkey( &ctx, key, 256 );

	esp_aes_crypt_cbc( &ctx, ESP_AES_DECRYPT, sizeof(ciphertext), iv, (uint8_t*)ciphertext, (uint8_t*)plaintext );

	esp_aes_free( &ctx );
}