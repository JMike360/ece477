#include "esp_log.h"
#include "../include/fingerprint.h"

#define TAG "FINGERPRINT"

sensor_packet* createPacket(uint8_t pid, uint16_t length, uint8_t* data, uint16_t checksum){
    sensor_packet* pkt = malloc(sizeof(sensor_packet));
    pkt->header = PKT_HEADER;
    pkt->addr[0] = 0xFF;
    pkt->addr[1] = 0xFF;
    pkt->addr[2] = 0xFF;
    pkt->addr[3] = 0xFF;
    pkt->pid = pid;
    pkt->length = length;
    pkt->data = malloc(sizeof(uint8_t)*(length-2));
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

bool isEqual(sensor_packet* pkt1, sensor_packet* pkt2){
    bool isEqual = true;
    isEqual &= (pkt1->header == pkt2->header);
    isEqual &= (pkt1->addr[0] == pkt2->addr[0]);
    isEqual &= (pkt1->addr[1] == pkt2->addr[1]);
    isEqual &= (pkt1->addr[2] == pkt2->addr[2]);
    isEqual &= (pkt1->addr[3] == pkt2->addr[3]);
    isEqual &= (pkt1->pid == pkt2->pid);
    isEqual &= (pkt1->length == pkt2->length);
    for(int i = 0; i < pkt1->length-2; i++){
        if((i >= pkt2->length-2) || (!isEqual)){
            break;
        }
        isEqual &= (pkt1->data[i] == pkt2->data[i]);
    }
    isEqual &= (pkt1->checksum == pkt2->checksum);
    return isEqual;
}

void printPacket(sensor_packet* pkt){
    #ifdef _DEBUG
    ESP_LOGI(TAG, "Header: 0x%04x", pkt->header);
    ESP_LOGI(TAG, "Addr: 0x%02x 0x%02x 0x%02x 0x%02x", pkt->addr[0], pkt->addr[1], pkt->addr[2], pkt->addr[3]);
    ESP_LOGI(TAG, "PID: 0x%02x", pkt->pid);
    ESP_LOGI(TAG, "Length: 0x%04x", pkt->length);
    ESP_LOGI(TAG, "Data:");
    for(int i = 0; i < pkt->length-2; i++){
        if((i%8)==0){
            ESP_LOGI(TAG, "");
        }
        else{
            ESP_LOGI(TAG, " ");
        }
        ESP_LOGI(TAG, "0x%02x", pkt->data[i]);
    }
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "Checksum: 0x%04x", pkt->checksum);
    #endif
}

void processResponse(uint8_t response){
    #ifdef _DEBUG
    switch(response){
        case ACK_CMD_COMPLETE:
            ESP_LOGI(TAG, "Command completed successfully");
            break;
        case ACK_RCV_ERR:
            ESP_LOGI(TAG, "Error receiving packet");
            break;
        case ACK_NO_FINGER:
            ESP_LOGI(TAG, "No finger detected on sensor");
            break;
        case ACK_ENROLL_FAIL:
            ESP_LOGI(TAG, "Failed to enroll fingerprint");
            break;
        case ACK_IMG_UNCLEAR:
            ESP_LOGI(TAG, "Failed to generate char file - Image unclear");
            break;
        case ACK_IMG_SMALL:
            ESP_LOGI(TAG, "Failed to generate char file - Image too small");
            break;
        case ACK_NO_MATCH:
            ESP_LOGI(TAG, "Finger does not match");
            break;
        case ACK_MATCH_NOT_FOUND:
            ESP_LOGI(TAG, "Failed to find matching finger in library");
            break;
        case ACK_CHAR_MERGE_ERR:
            ESP_LOGI(TAG, "Failed to merge char files");
            break;
        case ACK_BAD_ADDR:
            ESP_LOGI(TAG, "PageID address is out of range for finger library");
            break;
        case ACK_TEMPLATE_ERR:
            ESP_LOGI(TAG, "Error reading template from library, or invalid template");
            break;
        case ACK_TEMP_UPLOAD_ERR:
            ESP_LOGI(TAG, "Error uploading template");
            break;
        case ACK_NO_DATA_ACCEPT:
            ESP_LOGI(TAG, "Sensor is not ready for additional data packets");
            break;
        case ACK_IMG_UPLOAD_ERR:
            ESP_LOGI(TAG, "Error uploading image");
            break;
        case ACK_DEL_TEMP_FAIL:
            ESP_LOGI(TAG, "Failed to delete requested template");
            break;
        case ACK_CLEAR_LIB_FAIL:
            ESP_LOGI(TAG, "Failed to clear fingerprint library");
            break;
        case ACK_BAD_PASSWORD:
            ESP_LOGI(TAG, "Operation failed due to incorrect password");
            break;
        case ACK_BAD_PRIMARY_IMG:
            ESP_LOGI(TAG, "Failed to generate image - primary image invalid");
            break;
        case ACK_FLASH_WRITE_ERR:
            ESP_LOGI(TAG, "Error when writing to flash");
            break;
        case ACK_NO_DEF_ERR:
            ESP_LOGI(TAG, "Undefined error");
            break;
        case ACK_INVALID_REG:
            ESP_LOGI(TAG, "Invalid register number");
            break;
        case ACK_BAD_REG_CONFIG:
            ESP_LOGI(TAG, "Incorrect register configuration");
            break;
        case ACK_BAD_NOTEPAD_PG:
            ESP_LOGI(TAG, "Invalid notepad page number");
            break;
        case ACK_COMM_PORT_FAIL:
            ESP_LOGI(TAG, "Failed to operate the communication port");
            break;
        case ACK_ADDR_ERR:
            ESP_LOGI(TAG, "Address error");
            break;
        case ACK_PWD_NO_VERIFIED:
            ESP_LOGI(TAG, "Error - passowrd not verified");
            break;
        default:
            ESP_LOGI(TAG, "Error - unknown response code");
            break;
    }
    #endif
}

int sendPacketAsByteStream(sensor_packet* pkt){
    if(pkt == NULL){
        return -1;
    }

    size_t streamSize = sizeof(pkt->header) +
                        sizeof(pkt->pid) +
                        sizeof(pkt->addr[0])*4 +
                        sizeof(pkt->length) +
                        sizeof(uint8_t)*(pkt->length);
    #ifdef _DEBUG
    ESP_LOGI(TAG, "Stream size: %d", (int)streamSize);
    #endif
    char* byteStream = malloc(streamSize);
    byteStream[0] = (const char)(pkt->header >> 8);
    byteStream[1] = (const char)(pkt->header & 0xFF);
    byteStream[2] = (const char) pkt->addr[0];
    byteStream[3] = (const char) pkt->addr[1];
    byteStream[4] = (const char) pkt->addr[2];
    byteStream[5] = (const char) pkt->addr[3];
    byteStream[6] = (const char) pkt->pid;
    byteStream[7] = (const char)(pkt->length >> 8);
    byteStream[8] = (const char)(pkt->length & 0xFF);
    int offset = 9;
    int i = offset;
    for(; i < offset+pkt->length-2; i++){
        byteStream[i] = pkt->data[i-offset];
    }
    byteStream[i] = (const char)(pkt->checksum >> 8);
    byteStream[i+1] = (const char)(pkt->checksum & 0xFF);
    /*ESP_LOGI(TAG, "Bytes sent:");
    for(int j = 0; j < i+2; j++){
        ESP_LOGI(TAG, "0x%02x", byteStream[j]);
    }*/
    int result = uart_write_bytes(UART_NUM_2, byteStream, streamSize);
    #ifdef _DEBUG
    if(result == -1){
        ESP_LOGI(TAG, "Parameter error when sending uart packet");
    }
    #endif
    free(byteStream);
    return result;
}

sensor_packet* recvPacketFromByteStream(int size){
    uint8_t* recv = calloc(size, sizeof(uint8_t));
    int buflen = 0;
    int waitCnt = 0;
    while(buflen < size){
        if(waitCnt >= 1000){
            ESP_LOGI(TAG, "Buffer timed out, continuing with buffer length %d...", buflen);
            break;
        }
        vTaskDelay(1);
        uart_get_buffered_data_len(UART_NUM_2, (size_t*)&buflen);
        waitCnt++;
    }
    uart_read_bytes(UART_NUM_2, recv, size, 20/portTICK_RATE_MS);
    sensor_packet* pkt = malloc(sizeof(sensor_packet));
    pkt->header  = (((uint16_t)recv[0]) << 8) | ((uint16_t)recv[1]);
    pkt->addr[0] = recv[2];
    pkt->addr[1] = recv[3];
    pkt->addr[2] = recv[4];
    pkt->addr[3] = recv[5];
    pkt->pid     = recv[6];
    pkt->length  = (((uint16_t)recv[7]) << 8) | ((uint16_t)recv[8]);
    pkt->length  = (pkt->length > 0)? pkt->length : 3;
    pkt->data    = malloc(sizeof(uint8_t) * (pkt->length - 2));
    int offset = 9;
    int i = offset;
    for(; i < offset+pkt->length-2; i++){
        pkt->data[i-offset] = recv[i];
    }
    pkt->checksum = (((uint16_t)recv[i]) << 8) | ((uint16_t)recv[i+1]);

    /*for(int j = 0; j < size; j++){
        ESP_LOGI(TAG, "recv: 0x%02x", recv[j]);
    }*/
    free(recv);
    return pkt;
}


//--------- Handshake-----------------------------------------------------//
int sendHandshakePacket(){
    uint8_t data[2] = {0x17, 0x00};
    sensor_packet* hs_pkt = createPacket(PKT_PID_CMD, 0x0004, data, 0x001C);
    int result = sendPacketAsByteStream(hs_pkt);
    uart_wait_tx_done(UART_NUM_2, 100);
    freePacket(hs_pkt);
    return result;
}

int recvHandshakeAck(){
    sensor_packet* pkt = recvPacketFromByteStream(12);
    printPacket(pkt);

    uint8_t response = pkt->data[0];
    processResponse(response);
    
    sensor_packet* expected = createPacket(0x07, 0x0003, pkt->data, pkt->checksum);
    if(!isEqual(pkt, expected)){
        ESP_LOGI(TAG, "Error - received packet does not match expected format");
        response = -1;
    }

    freePacket(expected); 
    freePacket(pkt);
    return response;
}

//----- Set System Parameter-----------------------------------------------//
int sendSetSystemParamPacket(uint8_t paramNumber, uint8_t content){
    uint8_t data[3] = {0x0C, paramNumber, content};
    uint16_t length = 0x0005;
    uint16_t checksum = PKT_PID_CMD + length;
    for(int i = 0; i < 3; i++){
        checksum += data[i];
    }
    sensor_packet* param_pkt = createPacket(PKT_PID_CMD, 0x0005, data, checksum);
    sendPacketAsByteStream(param_pkt);
    freePacket(param_pkt);
    return 0;
}

int recvSetSystemParamAck(){
    ESP_LOGI(TAG, "Not implemented");
    return 0;
}


//------ Read System Parameters --------------------------------------------//
int sendReadSystemParamPacket(){
    uint8_t data[] = { 0x0F };  
    sensor_packet* readParamPkt = createPacket(PKT_PID_CMD, 0x0003, data, 0x0013);
    int sendResult = sendPacketAsByteStream(readParamPkt);
    freePacket(readParamPkt);
    return sendResult;
}

int recvReadSystemParamAck(){
    int ackSize = 28;
    sensor_packet* pkt = recvPacketFromByteStream(ackSize);
    
    printPacket(pkt);
    sensor_packet* expected = createPacket(PKT_PID_ACK, 0x0013, pkt->data, pkt->checksum);

    uint8_t response = pkt->data[0];
    processResponse(response);

    if(!isEqual(pkt, expected)){
        ESP_LOGI(TAG, "Error - received packet does not match expected format");
        response = -1;
    }

    freePacket(expected); 
    freePacket(pkt);
    return response;

}

//------ Store Template --------------------------------------------------//
int sendStoreTemplatePacket(uint8_t bufferID, uint16_t pageID){
    uint8_t data[] = {0x06, bufferID, (uint8_t)(pageID>>8), (uint8_t)(pageID & 0xFF)};
    uint16_t checksum = PKT_PID_CMD + 0x0006 + data[0] + data[1] + data[2] + data[3];
    sensor_packet* storTempPkt = createPacket(PKT_PID_CMD, 0x0006, data, checksum);
    int result = sendPacketAsByteStream(storTempPkt);
    uart_wait_tx_done(UART_NUM_2, 100);
    freePacket(storTempPkt);
    return result;
}

int recvStoreTemplateAck(){
    sensor_packet* pkt = recvPacketFromByteStream(12);
    if(pkt == NULL){
        ESP_LOGI(TAG, "Error: null packet received");
        return -1;
    }
    printPacket(pkt);

    uint8_t response = pkt->data[0];
    processResponse(response);
    
    sensor_packet* expected = createPacket(PKT_PID_ACK, 0x0003, pkt->data, pkt->checksum);
    if(!isEqual(pkt, expected)){
        ESP_LOGI(TAG, "Error - received packet does not match expected format");
        response = -1;
    }


    freePacket(expected); 
    freePacket(pkt);
    return response;
}

//------ Generate Template -----------------------------------------------//
int sendGenerateTemplatePacket(){
    uint8_t data[] = { 0x05 };
    sensor_packet* genTempPkt = createPacket(PKT_PID_CMD, 0x0003, data, 0x09);
    int result = sendPacketAsByteStream(genTempPkt);
    uart_wait_tx_done(UART_NUM_2, 100);
    freePacket(genTempPkt);
    return result;
}

int recvGenerateTemplateAck(){
    sensor_packet* pkt = recvPacketFromByteStream(12);
    if(pkt == NULL){
        ESP_LOGI(TAG, "Error: null packet received");
        return -1;
    }
    printPacket(pkt);

    uint8_t response = pkt->data[0];
    processResponse(response);
    
    sensor_packet* expected = createPacket(PKT_PID_ACK, 0x0003, pkt->data, pkt->checksum);
    if(!isEqual(pkt, expected)){
        ESP_LOGI(TAG, "Error - received packet does not match expected format");
        response = -1;
    }


    freePacket(expected); 
    freePacket(pkt);
    return response;
}

//------- Generate Image -------------------------------------------------//
int sendGenerateImgPacket(){
    uint8_t data[] = { 0x01 };
    sensor_packet* genImgPkt = createPacket(PKT_PID_CMD, 0x0003, data, 0x0005);
    int result = sendPacketAsByteStream(genImgPkt);
    uart_wait_tx_done(UART_NUM_2, 100);
    freePacket(genImgPkt);
    return result;
}

int recvGenerateImgAck(){
    sensor_packet* pkt = recvPacketFromByteStream(12);
    if(pkt == NULL){
        ESP_LOGI(TAG, "Error: null packet received");
        return -1;
    }
    printPacket(pkt);

    uint8_t response = pkt->data[0];
    processResponse(response);
    
    sensor_packet* expected = createPacket(PKT_PID_ACK, 0x0003, pkt->data, pkt->checksum);
    if(!isEqual(pkt, expected)){
        ESP_LOGI(TAG, "Error - received packet does not match expected format");
        response = -1;
    }


    freePacket(expected); 
    freePacket(pkt);
    return response;
}

int captureImage(int timeout_ms){
    int response = -1;
    int cnt = 0;
    while((response != ACK_CMD_COMPLETE) && (cnt < timeout_ms)){
        vTaskDelay(1);
        sendGenerateImgPacket();
        response = recvGenerateImgAck();
        cnt++;
    }
    #ifdef _DEBUG
    if(response != ACK_CMD_COMPLETE) ESP_LOGI(TAG, "Error - request timed out");
    #endif

    return (response == ACK_CMD_COMPLETE)? 0 : -1;
}


//------- Create Character File From Image --------------------------------//
int sendGenerateFileFromImgPacket(uint8_t bufferID){
    uint8_t data[] = { 0x02, bufferID };
    uint16_t checksum = PKT_PID_CMD + 0x0004 + data[0] + data[1];
    
    sensor_packet* genFilePkt = createPacket(PKT_PID_CMD, 0x0004, data, checksum);
    
    int result = sendPacketAsByteStream(genFilePkt);

    freePacket(genFilePkt);
    return result;
}

int recvGenerateFileFromImgAck(){
    sensor_packet* pkt = recvPacketFromByteStream(12);
    if(pkt == NULL){
        ESP_LOGI(TAG, "Error: null packet received");
        return -1;
    }
    printPacket(pkt);

    uint8_t response = pkt->data[0];
    processResponse(response);
    
    sensor_packet* expected = createPacket(PKT_PID_ACK, 0x0003, pkt->data, pkt->checksum);
    if(!isEqual(pkt, expected)){
        ESP_LOGI(TAG, "Error - received packet does not match expected format");
        response = -1;
    }

    freePacket(expected); 
    freePacket(pkt);
    return response;
}

//-------- Search Library For Match ---------------------------------------------//
int sendSearchLibraryPacket(uint8_t bufferID, uint16_t startPage, uint16_t n){
    uint8_t data[] = { 0x04, bufferID, (uint8_t)(startPage>>8), (uint8_t)(startPage&0xFF),
                                       (uint8_t)(n >> 8), (uint8_t)(n & 0xFF) };
    uint16_t checksum = PKT_PID_CMD + 0x0008 + data[0] + data[1] + data[2] + data[3] + data[4] + data[5];
    
    sensor_packet* searchPkt = createPacket(PKT_PID_CMD, 0x0008, data, checksum);
    
    int result = sendPacketAsByteStream(searchPkt);

    freePacket(searchPkt);
    return result;

}

int recvSearchLibraryAck(){
    sensor_packet* pkt = recvPacketFromByteStream(16);
    if(pkt == NULL){
        ESP_LOGI(TAG, "Error: null packet received");
        return -1;
    }
    printPacket(pkt);

    uint8_t response = pkt->data[0];
    processResponse(response);
    
    sensor_packet* expected = createPacket(PKT_PID_ACK, 0x0007, pkt->data, pkt->checksum);
    if(!isEqual(pkt, expected)){
        if(response == 0x09){
            ESP_LOGI(TAG, "No match found.");
        }
        else{
            ESP_LOGI(TAG, "Error - received packet does not match expected format");
        }
        freePacket(expected);
        freePacket(pkt);
        return -1;
    }
    uint16_t pageID = (((uint16_t)pkt->data[1]) << 8) + ((uint16_t)pkt->data[2]);
    uint16_t matchScore = (((uint16_t)pkt->data[3]) << 8) + ((uint16_t)pkt->data[4]);
    ESP_LOGI(TAG, "Matching finger found with ID: %d and match score: %d", pageID, matchScore);

    freePacket(expected); 
    freePacket(pkt);
    return response;

}

//------- Load Template From Flash Storage -------------------------------------------//
int sendLoadTemplatePacket(uint8_t bufferID, uint16_t pageID){
    uint8_t data[] = { 0x07, bufferID, (uint8_t)(pageID>>8), (uint8_t)(pageID&0xFF) };
        
    uint16_t checksum = PKT_PID_CMD + 0x0006 + data[0] + data[1] + data[2] + data[3];
    
    sensor_packet* searchPkt = createPacket(PKT_PID_CMD, 0x0006, data, checksum);
    
    int result = sendPacketAsByteStream(searchPkt);

    freePacket(searchPkt);
    return result;
}

int recvLoadTemplateAck(){
    sensor_packet* pkt = recvPacketFromByteStream(12);
    if(pkt == NULL){
        ESP_LOGI(TAG, "Error: null packet received");
        return -1;
    }
    printPacket(pkt);

    uint8_t response = pkt->data[0];
    processResponse(response);
    uint8_t data[] = { 0x00 }; 
    uint16_t checksum = 0x000A;
    sensor_packet* expected = createPacket(PKT_PID_ACK, 0x0003, data, checksum);
    if(!isEqual(pkt, expected)){
        if(response == 0x0C){
            ESP_LOGI(TAG, "Error loading template, template may be invalid.");
        }
        else if (response == 0x0B){
            ESP_LOGI(TAG, "Fingerprint ID is outside of template library range.");
        }
        else{
            ESP_LOGI(TAG, "Error - received packet does not match expected format");
        }
        response = -1;
    }

    freePacket(expected); 
    freePacket(pkt);
    return response;
}

//------ Clear Library ----------------------------------------------------//
int sendClearLibraryPacket(){
    uint8_t data[] = { 0x0d };
    sensor_packet* clrLibPkt = createPacket(PKT_PID_CMD, 0x0003, data, 0x0011);
    
    int result = sendPacketAsByteStream(clrLibPkt);

    freePacket(clrLibPkt);
    return result;
}

int recvClearLibraryAck(){
    sensor_packet* pkt = recvPacketFromByteStream(12);
    if(pkt == NULL){
        ESP_LOGI(TAG, "Error: null packet received");
        return -1;
    }
    printPacket(pkt);

    uint8_t response = pkt->data[0];
    processResponse(response);
    uint8_t data[] = { 0x00 }; 
    uint16_t checksum = 0x000A;
    sensor_packet* expected = createPacket(PKT_PID_ACK, 0x0003, data, checksum);
    if(!isEqual(pkt, expected)){
        if(response == 0x11){
            ESP_LOGI(TAG, "Failed to clear fingerprint library.");
        }
        else{
            ESP_LOGI(TAG, "Error - received packet does not match expected format");
        }
        response = -1;
    }

    freePacket(expected); 
    freePacket(pkt);
    return response;
}

//----- Check if buffer 1 and buffer 2 are matching fingerprints ---------------------------//
int sendCheckMatchPacket(){
    uint8_t data[] = { 0x03 };
    sensor_packet* chkMatchPkt = createPacket(PKT_PID_CMD, 0x0003, data, 0x0007);
    
    int result = sendPacketAsByteStream(chkMatchPkt);

    freePacket(chkMatchPkt);
    return result;
}

int recvCkeckMatchAck(){
    sensor_packet* pkt = recvPacketFromByteStream(14);
    if(pkt == NULL){
        ESP_LOGI(TAG, "Error: null packet received");
        return -1;
    }
    printPacket(pkt);

    uint8_t response = pkt->data[0];
    processResponse(response);
    uint8_t data[] = { 0x00 }; 
    uint16_t checksum = 0x000A;
    sensor_packet* expected = createPacket(PKT_PID_ACK, 0x0003, data, checksum);
    if(!isEqual(pkt, expected)){
        if(response == 0x11){
            ESP_LOGI(TAG, "Failed to clear fingerprint library.");
        }
        else{
            ESP_LOGI(TAG, "Error - received packet does not match expected format");
        }
       response = -1;
    }

    freePacket(expected); 
    freePacket(pkt);
    return response;
}

//------- Upload Char File from FP sensor to Microproc ----------------------------//
int sendUploadFilePacket(uint8_t bufferID){
    uint8_t data[] = { 0x08, bufferID };
    uint16_t checksum = PKT_PID_CMD + 0x0004 + data[0] + data[1];
    sensor_packet* upldFilePkt = createPacket(PKT_PID_CMD, 0x0004, data, checksum);
    
    int result = sendPacketAsByteStream(upldFilePkt);

    freePacket(upldFilePkt);
    return result;

}

int recvUploadFileAck(uint8_t** charFile, int* size){
    sensor_packet** pktList = malloc(sizeof(sensor_packet*)*20);
    sensor_packet* pkt = recvPacketFromByteStream(12);
    int idx = 0;
    int timeout = 19;
    while(pkt->pid != PKT_PID_END){
        if(pkt->header == 0x0000){
            break;
        }
        pktList[idx] = pkt;
        pkt = recvPacketFromByteStream(139);
        idx++;
        if(idx >= timeout){
            break;
        }
    }
    pktList[idx] = pkt;

    size_t refSize = (sizeof(uint8_t)*128*idx);
    *charFile = malloc(refSize);
    *size = (int)refSize;
    int fileIdx = 0;

    ESP_LOGI(TAG, "Received %d total packets, %d data packets and 1 ack packet", idx+1, idx);
    uint8_t response = 0;
    for(int i = 0; i <= idx; i++){
        printPacket(pktList[i]);
        if(i == 0){
            response = pktList[i]->data[0];
            processResponse(response);
            uint8_t data[] = { 0x00 }; 
            uint16_t checksum = 0x000A;
            sensor_packet* expected = createPacket(PKT_PID_ACK, 0x0003, data, checksum);
            if(!isEqual(pktList[i], expected)){
                if(response == 0x0F){
                    ESP_LOGI(TAG, "Error - Failed to transfer char file.");
                }
                else{
                    ESP_LOGI(TAG, "Error - received packet does not match expected format");
                }
                response = -1;
            }
            freePacket(expected); 
        }
        else{
            for(int pktIdx = 0; pktIdx < (pktList[i]->length-2); pktIdx++){
                if(fileIdx >= *size){
                    ESP_LOGI(TAG, "ERROR --- charFile expected size exceeded ---------------------------------------");
                    break;
                }
                (*charFile)[fileIdx] = pktList[i]->data[pktIdx];
                fileIdx++;
            }
        }
    }

    for(int i = 0; i <= idx; i++){
        freePacket(pktList[i]);
    }
    free(pktList);
    return response;

}

//-------- LED Control Functions ------------------------------------------------------//
int sendTurnLedOnPacket(){
    uint8_t data[] = { 0x50 };
    uint16_t checksum = PKT_PID_CMD + 0x0003 + data[0];
    sensor_packet* ledOnPkt = createPacket(PKT_PID_CMD, 0x0003, data, checksum);
    int result = sendPacketAsByteStream(ledOnPkt);

    freePacket(ledOnPkt);
    return result;
}

int recvTurnLedOnAck(){
    sensor_packet* pkt = recvPacketFromByteStream(12);
    if(pkt == NULL){
        ESP_LOGI(TAG, "Error: null packet received");
        return -1;
    }
    printPacket(pkt);

    uint8_t response = pkt->data[0];
    processResponse(response);
    uint8_t data[] = { 0x00 }; 
    uint16_t checksum = 0x000A;
    sensor_packet* expected = createPacket(PKT_PID_ACK, 0x0003, data, checksum);
    if(!isEqual(pkt, expected)){
        ESP_LOGI(TAG, "Error - received packet does not match expected format");
        response = -1;
    }

    freePacket(expected); 
    freePacket(pkt);
    return response;
}

int sendTurnLedOffPacket(){
    uint8_t data[] = { 0x51 };
    uint16_t checksum = PKT_PID_CMD + 0x0003 + data[0];
    sensor_packet* ledOffPkt = createPacket(PKT_PID_CMD, 0x0003, data, checksum);
    int result = sendPacketAsByteStream(ledOffPkt);

    freePacket(ledOffPkt);
    return result;
}

int recvTurnLedOffAck(){
    sensor_packet* pkt = recvPacketFromByteStream(12);
    if(pkt == NULL){
        ESP_LOGI(TAG, "Error: null packet received");
        return -1;
    }
    printPacket(pkt);

    uint8_t response = pkt->data[0];
    processResponse(response);
    uint8_t data[] = { 0x00 }; 
    uint16_t checksum = 0x000A;
    sensor_packet* expected = createPacket(PKT_PID_ACK, 0x0003, data, checksum);
    if(!isEqual(pkt, expected)){
        ESP_LOGI(TAG, "Error - received packet does not match expected format");
        response = -1;
    }

    freePacket(expected); 
    freePacket(pkt);
    return response;
}

//------ High Level Functions -------------------------------------------------------//

//returns 1 if enrolled fingerprint template is found, 0 otherwise
int checkFingerEnrolled(){
    sendLoadTemplatePacket(0x01, 0x00); //attempt to load template 0 to buffer 1
    int resp = recvLoadTemplateAck();
    
    //turn off led
    sendTurnLedOffPacket();
    int ledResp = recvTurnLedOffAck();
    if(ledResp != 0){
        ESP_LOGE(TAG, "Failed to turn off led...");
    }

    if (resp == 0)
        ESP_LOGI(TAG, "Fingerprint is enrolled");
    else
        ESP_LOGI(TAG, "Fingerprint is not enrolled");

    return (resp == 0)? 1 : 0;
}

//returns 0 on success, -1 otherwise
int enrollFinger(int templateID){
    sendHandshakePacket();
    int hsResp = recvHandshakeAck();
    if(hsResp != 0){
        ESP_LOGE(TAG, "Handshake failed...");
        return -1;
    }

    captureImage(1000);
    sendGenerateFileFromImgPacket(0x01);
    int genFileResp1 = recvGenerateFileFromImgAck();
    if(genFileResp1 != 0){
        ESP_LOGE(TAG, "Failed to generate charFile for buffer 1...");
        return -1;
    }

    captureImage(1000);
    sendGenerateFileFromImgPacket(0x02);
    int genFileResp2 = recvGenerateFileFromImgAck();
    if(genFileResp2 != 0){
        ESP_LOGE(TAG, "Failed to generate charFile for buffer 2...");
        return -1;
    }

    sendGenerateTemplatePacket();
    int genTmpResp = recvGenerateTemplateAck();
    if(genTmpResp != 0){
        ESP_LOGE(TAG, "Failed to generate template...");
        return -1;
    }

    uint16_t pageID = (uint16_t)templateID;
    sendStoreTemplatePacket(0x01, pageID);
    int storResp = recvStoreTemplateAck();
    if(storResp != 0){
        ESP_LOGE(TAG, "Failed to store template...");
        return -1;
    }

    //turn off led
    sendTurnLedOffPacket();
    int ledResp = recvTurnLedOffAck();
    if(ledResp != 0){
        ESP_LOGE(TAG, "Failed to turn off led...");
    }

    ESP_LOGI(TAG, "Fingerprint enrollment successful");

    return 0;
} 

//returns 1 if finger matches, 0 otherwise
// allow 3 tries
int authenticateFinger(){
    int res = 0;
    int numtry = 0;
    do {
        sendHandshakePacket();
        int hsResp = recvHandshakeAck();
        if(hsResp != 0){
            ESP_LOGE(TAG, "Handshake failed...");
        }

        captureImage(1000);
        sendGenerateFileFromImgPacket(0x01);
        int genFileResp1 = recvGenerateFileFromImgAck();
        if(genFileResp1 != 0){
            ESP_LOGE(TAG, "Failed to generate charFile for buffer 1... resp = %d", genFileResp1);
        }

        sendSearchLibraryPacket(0x01, 0x0000, 0x0005);
        int libSrchResp = recvSearchLibraryAck();
        if(libSrchResp == 0){
            ESP_LOGI(TAG, "Fingerprint authentication successful");
            res = 1;
        }

        //turn off led
        sendTurnLedOffPacket();
        int ledResp = recvTurnLedOffAck();
        if(ledResp != 0){
            ESP_LOGE(TAG, "Failed to turn off led...");
        }
    } while (res == 0 && ++numtry < 3);

    return res;
}

//Params: uint8_t** key is the pointer to a uint8_t* to which digest data will be assigned (can be NULL, must be free()'d later)
                                               //        int* keySize is a pointer whose referenced value will be assigned the digest size
                                               //returns 0 for success, -1 otherwise
int getCryptoKey(uint8_t** key, int* keySize){
    uint8_t bufferID = 0x01;
    sendUploadFilePacket(bufferID);
    uint8_t* charFile = NULL;
    int size = -1;
    int resp = recvUploadFileAck(&charFile, &size);
    if(resp != 0){
        #ifdef _DEBUG
        ESP_LOGE(TAG, "Received failure code, aborting test...");
        #endif
        return -1;
    }
    if((charFile == NULL) || (size == -1)){
        #ifdef _DEBUG
        ESP_LOGE(TAG, "Char file failed to populate, aborting test...");
        #endif
        return -1;
    }

    size_t inSize = sizeof(uint8_t)*size;
    int result = getHashedCryptoKey(charFile, inSize, key, keySize);
    #ifdef _DEBUG
    ESP_LOGE(TAG, "Key hash result: %d", result);
    #endif
    if((*key == NULL) || (*keySize == -1)){
        #ifdef _DEBUG
        ESP_LOGE(TAG, "Hashed key failed to populate, aborting test...");
        #endif
        return -1;
    }
    
    #ifdef _DEBUG
    ESP_LOGI(TAG, "Hashed Crypto Key:");
    for(int i = 0; i < *keySize; i++){
        if((i%8) == 0){
            ESP_LOGI(TAG, "");
        }
        ESP_LOGI(TAG, "0x%02x ", (*key)[i]);
    }
    #endif
    
    //turn off led
    sendTurnLedOffPacket();
    int ledResp = recvTurnLedOffAck();
    if(ledResp != 0){
        #ifdef _DEBUG
        ESP_LOGE(TAG, "Failed to turn off led...");
        #endif
    }

    ESP_LOGI(TAG, "Sucessfully hashed crypto key");
    return 0;
} 

//returns 0 on success, -1 otherwise
int clearAllData(){
    sendClearLibraryPacket();
    int clrResp = recvClearLibraryAck();
    
    //turn off led
    sendTurnLedOffPacket();
    int ledResp = recvTurnLedOffAck();
    if(ledResp != 0){
        #ifdef _DEBUG
        ESP_LOGE(TAG, "Failed to turn off led...");
        #endif
    }
    if (clrResp == 0)
        ESP_LOGI(TAG, "Successfully cleared all data");
    else
        ESP_LOGE(TAG, "Failed to clear all data");

    return (clrResp == 0)? 0 : -1;
}
