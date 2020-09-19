#ifndef __SDCARD_H__
#define __SDCARD_H__

#include "esp_err.h"
#include "esp_log.h"

#define SD_CMD_LEN          40
#define SD_START            ((long)0x1 << 38)

// SD command index
#define SD_INDEX_POS        32 // shift left by this many bits
#define GO_IDLE_STATE       0x0
#define SEND_OP_COND        0x1
#define SEND_IF_COND        0x8
#define SEND_CSD            0x9
#define SEND_CID            0xa
#define STOP_TRANSMISSION   0xc
#define SET_BLOCKLEN        0x10
#define READ_SINGLE_BLOCK   0x11
#define READ_MULTIPLE_BLOCK 0x12
#define SET_BLOCK_COUNT     0x17
#define WRITE_BLOCK         0x18
#define WRITE_MULTIPLE_BLOCK 0x19
#define APP_CMD             0x37
#define READ_OCR            0x3a

#endif