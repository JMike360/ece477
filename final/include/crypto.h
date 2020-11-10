#include <string.h>
#include <stdio.h>
#include <mbedtls/aes.h>
#include <esp32/rom/aes.h>

// plaintext and cipertext should be 
char plaintext[16384];
char ciphertext[16384];

// high level functions to be called from other places
void aes_encrypt(uint8_t* plaintext, uint8_t* key, uint8_t* ciphertext);
void aes_decrypt(uint8_t* ciphertext, uint8_t* key, uint8_t* plaintext);