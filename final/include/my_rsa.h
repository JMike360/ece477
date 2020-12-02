#ifndef __RSA_H__
#define __RSA_H__

#include <stdint.h>

#define RSA_KEYLEN_IN_BITS (2048)
#define RSA_KEYLEN_IN_BYTES (RSA_KEYLEN_IN_BITS / 8)

#define RSA_SUCCESS 1
#define RSA_FAILURE 0

int my_rsa_init();
int isKeyReceived();
int my_rsa_key_send();
int my_rsa_key_recv(uint8_t* data);
int my_rsa_encrypt(uint8_t* plaintext, uint8_t** ciphertext);
int my_rsa_decrypt(uint8_t* ciphertext, uint8_t** plaintext);
int my_rsa_deinit();

#endif