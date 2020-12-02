#ifndef __RSA_H__
#define __RSA_H__

#include <stdint.h>

void my_rsa_init();
void my_rsa_encrypt(uint8_t* plaintext, uint8_t* ciphertext);
void my_rsa_decrypt(uint8_t* ciphertext, uint8_t* plaintext);
void my_rsa_deinit();

#endif