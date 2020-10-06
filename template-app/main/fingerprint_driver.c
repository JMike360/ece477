#include "fingerprint_driver.h"

sensor_packet* createPacket(uint8_t pid, uint16_t length, uint8_t* data, uint16_t checksum){
    sensor_packet* pkt = malloc(sizeof(sensor_packet));
    pkt->header = PKT_HEADER;
    pkt->addr[0] = 0xFF;
    pkt->addr[1] = 0xFF;
    pkt->addr[2] = 0xFF;
    pkt->addr[3] = 0xFF;
    pkt->pid = pid;
    pkt->length = length;
    pkt->data = malloc(sizeof(uint8_t)*length);
    for(int i = 0; i < length-2; i++){
        pkt->data[i] = data[i];
    }
    pkt->checksum = checksum;
    return pkt;
}

void freePacket(sensor_packet* pkt){
    free(pkt->data);
    free(pkt);
}

void sendPacketAsByteStream(sensor_packet* pkt){
    const char* byteStream = (const char*) pkt;
    uart_write_bytes(PORT_NUM, byteStream, sizeof(pkt));
}

int sendHandshakePacket(){
    uint8_t data[2] = {0x17, 0x00};
    sensor_packet* hs_pkt = createPacket(PKT_PID_CMD, 0x0004, data, 0x001C);
    sendPacketAsByteStream(hs_pkt);
    return 0;
}

int recvHandshakeAck(){
    return 0;
}

int sendSetSystemParamPacket(uint8_t paramNumber, uint8_t content){
    uint8_t data[3] = {0x0C, paramNumber, content};
    uint16_t length = 0x0005;
    uint16_t checksum = PKT_PID_CMD + length;
    for(int i = 0; i < 3; i++){
        checksum += data[i];
    }
    sensor_packet* param_pkt = createPacket(PKT_PID_CMD, 0x0005, data, checksum);
    sendPacketAsByteStream(param_pkt);
    return 0;
}

int recvSetSystemParamAck(){
    return 0;
}