#include "esp_system.h"
#include "uart_setup.h"
#include "crypto.h"

#define PORT_NUM PORT_NUM_2
#define DATA_PKT_SIZE 128

#define PKT_HEADER   0xEF01
#define PKT_ADDR    0xFFFFFFFF
#define PKT_PID_CMD  0x01
#define PKT_PID_DAT  0x02
#define PKT_PID_ACK  0x07
#define PKT_PID_END  0x08
#define PKT_LENGTH   0 // Length of packet content (CMD and DATA packets) plus length of checksum (2bytes) Measured in bytes
                     // Max length of 256 bytes. High byte transferred first
#define PKT_DATA     0   //Commands, data, command params, ack result, etc.
#define PKT_CHECKSUM 0 //2 bytes, arithmetic sum of PID, packet length, and all packet contents. Overflowing bits omitted, high byte transferred first

#define ACK_CMD_COMPLETE    0x00
#define ACK_RCV_ERR         0x01
#define ACK_NO_FINGER       0x02
#define ACK_ENROLL_FAIL     0x03
#define ACK_IMG_UNCLEAR     0x06
#define ACK_IMG_SMALL       0x07
#define ACK_NO_MATCH        0x08
#define ACK_MATCH_NOT_FOUND 0x09
#define ACK_CHAR_MERGE_ERR  0x0A
#define ACK_BAD_ADDR        0x0B
#define ACK_TEMPLATE_ERR    0x0C
#define ACK_TEMP_UPLOAD_ERR 0x0D
#define ACK_NO_DATA_ACCEPT  0x0E
#define ACK_IMG_UPLOAD_ERR  0x0F
#define ACK_DEL_TEMP_FAIL   0x10
#define ACK_CLEAR_LIB_FAIL  0x11
#define ACK_BAD_PASSWORD    0x13
#define ACK_BAD_PRIMARY_IMG 0x15
#define ACK_FLASH_WRITE_ERR 0x18
#define ACK_NO_DEF_ERR      0x19
#define ACK_INVALID_REG     0x1A
#define ACK_BAD_REG_CONFIG  0x1B
#define ACK_BAD_NOTEPAD_PG  0x1C
#define ACK_COMM_PORT_FAIL  0x1D
#define ACK_ADDR_ERR        0x20
#define ACK_PWD_NO_VERIFIED 0x21
//RX/TX pins on the feather board are labelled as 16RX - GPIO3 / 17TX - GPIO1 for the UART0 port

#define _DEBUG

typedef struct sensor_packet {
    uint16_t header;
    uint8_t addr[4];
    uint8_t  pid;
    uint16_t length;
    uint8_t* data;
    uint16_t checksum;
} sensor_packet;

sensor_packet* createPacket(uint8_t pid, uint16_t length, uint8_t* data, uint16_t checksum);
void freePacket(sensor_packet* pkt);

//-------------------------------------------------//
//------------ High Level Sensor Functions --------//
//-------------------------------------------------//

int captureImage(int timeout_ms);
int checkFingerEnrolled(); //returns 1 if enrolled fingerprint template is found, 0 otherwise
int enrollFinger(int templateID); //returns 0 on success, -1 otherwise
int authenticateFinger(); //returns 1 if finger matches, 0 otherwise
int getCryptoKey(uint8_t** key, int* keySize); //Params: uint8_t** key is the pointer to a uint8_t* to which digest data will be assigned (can be NULL, must be free()'d later)
                                               //        int* keySize is a pointer whose referenced value will be assigned the digest size
                                               //returns 0 for success, -1 otherwise
int clearAllData(); //returns 0 on success, -1 otherwise

//-------------------------------------------------//
//------------ Low Level Packet Functions ---------//
//-------------------------------------------------//

int sendHandshakePacket();
int recvHandshakeAck();

int sendSetSystemParamPacket(uint8_t paramNumber, uint8_t content);
int recvSetSystemParamAck();

int sendReadSystemParamPacket();
int recvReadSystemParamAck();

int sendReadTemplateNumPacket();
int recvReadTemplateNumAck();

int sendGenerateImgPacket();
int recvGenerateImgAck();

int sendUploadImgPacket();
int recvUploadImgAck();

int sendDownloadImgPacket();
int recvDownloadImgAck();

int sendGenerateFileFromImgPacket(uint8_t bufferID);
int recvGenerateFileFromImgAck();

int sendGenerateTemplatePacket();
int recvGenerateTemplateAck();

int sendUploadFilePacket(uint8_t bufferID);
int recvUploadFileAck(uint8_t** charFile, int* size);

int sendDownloadFilePacket();
int recvDownloadFileAck();

int sendStoreTemplatePacket(uint8_t bufferID, uint16_t pageID);
int recvStoreTemplateAck();

int sendLoadTemplatePacket(uint8_t bufferID, uint16_t pageID);
int recvLoadTemplateAck();

int sendDeleteTemplatePacket();
int recvDeleteTemplateAck();

int sendClearLibraryPacket();
int recvClearLibraryAck();

int sendCheckMatchPacket();
int recvCheckMatchAck();

int sendSearchLibraryPacket(uint8_t bufferID, uint16_t startPage, uint16_t n);
int recvSearchLibraryAck();

int sendGenerateRandomNumPacket();
int recvGenerateRandomNumAck();

int sendWriteNotepadPacket();
int recvWriteNotepadAck();

int sendReadNotepadPacket();
int recvReadNotepadAck();

int sendTurnLedOnPacket();
int recvTurnLedOnAck();

int sendTurnLedOffPacket();
int recvTurnLedOffAck();