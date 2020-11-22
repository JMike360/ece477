#ifndef __CRYPTO_H__
#define __CRYPTO_H__

#include <string.h>
#include <stdio.h>
#include <esp32/aes.h>
#include <mbedtls/sha256.h>

// plaintext and cipertext should be 
// char plaintext[16384];
// char ciphertext[16384];

// high level functions to be called from other places
void my_aes_encrypt(uint8_t* plaintext, uint8_t* key, uint8_t* ciphertext);
void my_aes_decrypt(uint8_t* ciphertext, uint8_t* key, uint8_t* plaintext);
int getHashedCryptoKey(uint8_t* input, size_t inputSize, uint8_t** key, int* keySize);

#endif