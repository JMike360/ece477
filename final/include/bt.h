#ifndef __BT_H__
#define __BT_H__

#include <stdint.h>

#define ENCRYPT_OFF 0
#define ENCRYPT_ON 1

void btInit();
void btRegister();
void btSetPairing();
void btSendData(uint8_t* data, int encryptMsg, int len);

#endif