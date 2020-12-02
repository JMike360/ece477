//----- Check if buffer 1 and buffer 2 are matching fingerprints ---------------------------//
int sendCheckMatchPacket(){
    uint8_t data[] = { 0x03 };
    sensor_packet* chkMatchPkt = createPacket(PKT_PID_CMD, 0x0003, data, 0x0007);
    
    int result = sendPacketAsByteStream(chkMatchPkt);

    freePacket(chkMatchPkt);
    return result;
}

int recvCheckMatchAck(){
    sensor_packet* pkt = recvPacketFromByteStream(14);
    if(pkt == NULL){
        printf("Error: null packet received\n");
        return -1;
    }
    printPacket(pkt);

    uint8_t response = pkt->data[0];
    processResponse(response);
    sensor_packet* expected = createPacket(PKT_PID_ACK, 0x0005, pkt->data, pkt->checksum);
    if(!isEqual(pkt, expected)){
        if(response == 0x08){
            printf("Buffers 1 and 2 do not match.\n");
        }
        else{
            printf("Error - received packet does not match expected format\n");
        }
       response = -1;
       freePacket(expected);
       freePacket(pkt);
       return response;
    }
    uint16_t matchScore = (((uint16_t)pkt->data[1]) << 8) + ((uint16_t)pkt->data[2]);
    printf("Buffers 1 and 2 match with match score: %d\n", matchScore);
    
    freePacket(expected); 
    freePacket(pkt);
    return response;
}

int authenticateFinger(){ 
    uart_begin(PORT_NUM_2);
    
    sendHandshakePacket();
    int hsResp = recvHandshakeAck();
    if(hsResp != 0){
        #ifdef _DEBUG
        printf("Handshake failed...\n");
        #endif
        return 0;
    }

    captureImage(1000);
    sendGenerateFileFromImgPacket(0x01);
    int genFileResp1 = recvGenerateFileFromImgAck();
    if(genFileResp1 != 0){
        #ifdef _DEBUG
        printf("Failed to generate charFile for buffer 1...\n");
        #endif
        return 0;
    }

    sendLoadTemplatePacket(0x02, 0x0000);
    int loadResp = recvLoadTemplateAck();
    if(loadResp != 0){
        printf("Failed to load template to buffer 2...0\n");
        return 0;
    }

    sendCheckMatchPacket();
    int matchResp = recvCheckMatchAck();
    if(matchResp == 0){
        printf("Match found\n");
        //turn off led
        sendTurnLedOffPacket();
        int ledResp = recvTurnLedOffAck();
        if(ledResp != 0){
            #ifdef _DEBUG
            printf("Failed to turn off led...\n");
            #endif
        }
        return 1;
    }


        //turn off led
    sendTurnLedOffPacket();
    int ledResp = recvTurnLedOffAck();
    if(ledResp != 0){
        #ifdef _DEBUG
        printf("Failed to turn off led...\n");
        #endif
    }
    return 0;
}



