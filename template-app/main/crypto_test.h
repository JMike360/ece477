#include <string.h>
#include <stdio.h>
#include <mbedtls/aes.h>
#include <mbedtls/sha256.h>

int encodetest();
int getHashedCryptoKey(uint8_t* input, size_t inputSize, uint8_t** key, int* keySize);