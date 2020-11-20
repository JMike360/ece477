#ifndef __BT_H__
#define __BT_H__

#include <stdint.h>

void btInit();
void btRegister();
void btSetPairing();
void btSendData(uint8_t* data);

#endif