#include <string.h>
#include <stdio.h>
#include <mbedtls/aes.h>
#include <mbedtls/sha256.h>

/* 
  For  Encryption time: 1802.40us  (9.09 MB/s) at 16kB blocks.
*/

static inline int32_t _getCycleCount(void) {
  int32_t ccount;
  __asm__ __volatile__("rsr %0,ccount":"=a" (ccount));
  return ccount;
}

char plaintext[16384];
char ciphertext[16384];


int encodetest()
{
	uint8_t key[32];
	uint8_t iv[16];

  //If you have cryptographically random data in the start of your payload, you do not need
	//an IV.  If you start a plaintext payload, you will need an IV.
	memset( iv, 0, sizeof( iv ) );

	//Right now, I am using a key of all zeroes.  This should change.  You should fill the key
  //out with actual data. 
	memset( key, 0, sizeof( key ) );

	memset( plaintext, 0, sizeof( plaintext ) );
	strcpy( plaintext, "Hello, world, how are you doing today?" );

	//Just FYI - you must be encrypting/decrypting data that is in BLOCKSIZE chunks!!!

	esp_aes_context ctx;
	esp_aes_init( &ctx );
	esp_aes_setkey( &ctx, key, 256 );
	int32_t start = _getCycleCount();
	esp_aes_crypt_cbc( &ctx, ESP_AES_ENCRYPT, sizeof(plaintext), iv, (uint8_t*)plaintext, (uint8_t*)ciphertext );
	int32_t end = _getCycleCount();
	float enctime = (end-start)/240.0;
	printf( "Encryption time: %.2fus  (%f MB/s)\n", enctime, (sizeof(plaintext)*1.0)/enctime );

	//See encrypted payload, and wipe out plaintext.
	memset( plaintext, 0, sizeof( plaintext ) );
	printf("Ciphertext:\n");
	int i;
	for( i = 0; i < 128; i++ )
	{
		printf( "%02x[%c]%c", ciphertext[i], (ciphertext[i]>31)?ciphertext[i]:' ', ((i&0xf)!=0xf)?' ':'\n' );
	}
	printf( "\n" );
	//Must reset IV.
	//XXX TODO: Research further: I found out if you don't reset the IV, the first block will fail
	//but subsequent blocks will pass.  Is there some strange cryptoalgebra going on that permits this?
	printf( "IV: %02x %02x\n", iv[0], iv[1] );
	memset( iv, 0, sizeof( iv ) );  


	//Use the ESP32 to decrypt the CBC block.
	esp_aes_crypt_cbc( &ctx, ESP_AES_DECRYPT, sizeof(ciphertext), iv, (uint8_t*)ciphertext, (uint8_t*)plaintext );


	//Verify output
	printf("Decrypted plaintext:\n");
	for( i = 0; i < 128; i++ )
	{
		printf( "%02x[%c]%c", plaintext[i],  (plaintext[i]>31)?plaintext[i]:' ', ((i&0xf)!=0xf)?' ':'\n' );
	}
	printf( "\n" );


	esp_aes_free( &ctx );
    return 0;
}


int getHashedCryptoKey(uint8_t* input, size_t inputSize, uint8_t** key, int* keySize){
	unsigned char* digest = malloc(sizeof(unsigned char)*32);
	int ret = mbedtls_sha256_ret((const unsigned char *)input, inputSize, digest, 0);
	*key = (uint8_t*)digest;
	*keySize = 32;
	return ret;
}