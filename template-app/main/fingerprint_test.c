
#define PKT_HEADER 0xEF01
#define PKT_ADDER 0xFFFFFFFF
#define PKT_PID_CMD 0x01
#define PKT_PID_DAT 0x02
#define PKT_PID_ACK 0x07
#define PKT_PID_END 0x08
#define PKT_LENGTH 0 // Length of packet content (CMD and DATA packets) plus length of checksum (2bytes) Measured in bytes
                     // Max length of 256 bytes. High byte transferred first
#define PKT_DATA 0   //Commands, data, command params, ack result, etc.
#define PKT_CHECKSUM 0 //2 bytes, arithmetic sum of PID, packet length, and all packet contents. Overflowing bits omitted, high byte transferred first

//RX/TX pins on the feather board are labelled as 16RX - GPIO3 / 17TX - GPIO1 for the UART0 port
#define PIN_TX 17
#define PIN_RX 16
#define BAUD_RATE 57600
#define PORT_NUM 2
#define STACK_SIZE 2048
#define BUF_SIZE 1024

#include "driver/uart.h"
#include "uart_setup.h"
#include "fingerprint_driver.h"

void setupFtdiConsole(){
    uart_begin(PORT_NUM_1);
}

int getFtdiCommand(){
    char prompt[] = "\r\n>>";
    uart_write_bytes(PORT_NUM_1, prompt, sizeof(char)*4);

    uint8_t* data = (uint8_t*) calloc(BUF_SIZE, sizeof(uint8_t));
    char cmd[50];
    int idx = 0;

    while(((char)data[0] != '\n') && ((char)data[0] != '\r')){
        int len = uart_read_bytes(PORT_NUM_1, data, BUF_SIZE, 20 / portTICK_RATE_MS);
        uart_write_bytes(PORT_NUM_1, (const char *)data, len);

        if((data[0] >= 33) && (data[0] <= 126)){
            cmd[idx] = data[0];
            idx++;
            if(idx == 49){
                break;
            }
            data[0] = 0;
        }
    }
    cmd[idx] = '\0';
    int command = atoi(cmd);
    return command;
}

int testUartDoubleInstall(){
    uart_begin(PORT_NUM_1);
    uart_begin(PORT_NUM_2);
    vTaskDelay(1);
    printf("Stdout still working...\n");
    //uart_end(PORT_NUM_0);
    uart_end(PORT_NUM_2);
    uart_end(PORT_NUM_1);
    return 0;
}

int uartEchoTest () {
    uart_config_t config = {
        .baud_rate = BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB
    };
    int intr_alloc_flags = 0;
    ESP_ERROR_CHECK(uart_driver_install(PORT_NUM, BUF_SIZE*2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(PORT_NUM, &config));
    ESP_ERROR_CHECK(uart_set_pin(PORT_NUM, PIN_TX, PIN_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    uint8_t* data = (uint8_t*) malloc(BUF_SIZE);
    while(1){
        int len = uart_read_bytes(PORT_NUM, data, BUF_SIZE, 20 / portTICK_RATE_MS);
        //printf("%s\n", (const char*) data);
        //printf("%d\n", len);
        uart_write_bytes(PORT_NUM, (const char *)data, len);
    }
    return 0;
}

int builtinEchoTest () {
    uart_config_t config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB
    };
    int intr_alloc_flags = 0;
    ESP_ERROR_CHECK(uart_driver_install(0, BUF_SIZE*2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(0, &config));
    ESP_ERROR_CHECK(uart_set_pin(0, 1, 3, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    uint8_t* data = (uint8_t*) malloc(BUF_SIZE);
    while(data[0] != '{'){
        int len = uart_read_bytes(0, data, BUF_SIZE, 20 / portTICK_RATE_MS);
        //printf("%s\n", (const char*) data);
        //printf("%d\n", len);
        uart_write_bytes(0, (const char *)data, len);
    }
    printf("\nFinished echo test\n");
    return 0;
}

int doubleEchoTest(){
    uart_config_t config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB
    };
    int intr_alloc_flags = 0;
    ESP_ERROR_CHECK(uart_driver_install(0, BUF_SIZE*2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(0, &config));
    ESP_ERROR_CHECK(uart_set_pin(0, 1, 3, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    uint8_t* data = (uint8_t*) malloc(BUF_SIZE);
    while(data[0] != '{'){
        int len = uart_read_bytes(0, data, BUF_SIZE, 20 / portTICK_RATE_MS);
        //printf("%s\n", (const char*) data);
        //printf("%d\n", len);
        uart_write_bytes(0, (const char *)data, len);
    }
    printf("\nFinished echo test\n");
    
    uart_config_t config2 = {
        .baud_rate = BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB
    };
    uart_driver_delete(0);
    intr_alloc_flags = 0;
    ESP_ERROR_CHECK(uart_driver_install(PORT_NUM, BUF_SIZE*2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(PORT_NUM, &config2));
    ESP_ERROR_CHECK(uart_set_pin(PORT_NUM, PIN_TX, PIN_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    free(data);
    data = (uint8_t*) malloc(BUF_SIZE);
    while(data[0] != '{'){
        int len = uart_read_bytes(PORT_NUM, data, BUF_SIZE, 20 / portTICK_RATE_MS);
        //printf("%s\n", (const char*) data);
        //printf("%d\n", len);
        uart_write_bytes(PORT_NUM, (const char *)data, len);
    }
    printf("\nFinished echo test\n");
    return 0;

}

int testLibraryHandshake(){
    uart_begin(PORT_NUM_2);
    int res = sendHandshakePacket();
    printf("Handshake send result: %d\n", res);
    recvHandshakeAck();
    return 0;
}

int testLibraryReadParam(){
    uart_begin(PORT_NUM_2);
    int res = sendReadSystemParamPacket();
    printf("Read param send result: %d\n", res);
    recvReadSystemParamAck();
    return 0;
}

int testLibraryGenImg(){
    uart_begin(PORT_NUM_2);
    int cnt = 0;
    while(cnt < 10){
        sendGenerateImgPacket();
        recvGenerateImgAck();
        cnt++;
    }
    return 0;
}

int testLibraryCaptureImage(){
    uart_begin(PORT_NUM_2);
    int result = captureImage(500);
    if (result == ACK_CMD_COMPLETE){
        printf("Image captured successfully\n");
    }
    else{
        printf("Failed to capture Image\n");
    }
    return 0;
}

int testLibraryGenerateFileFromImg(uint8_t bufferID){
    uart_begin(PORT_NUM_2);
    int res = sendGenerateFileFromImgPacket(bufferID);
    printf("Packet send result: %d\n", res);
    recvGenerateFileFromImgAck();
    return 0;
}

int testLibraryGenerateTemplate(){
    uart_begin(PORT_NUM_2);
    int res = sendGenerateTemplatePacket();
    printf("Packet send result: %d\n", res);
    recvGenerateTemplateAck();
    return 0;
}

int testLibraryMatchSearch(uint8_t bufferID){
    uart_begin(PORT_NUM_2);
    int res = sendSearchLibraryPacket(bufferID, 0x0000, 0x0005);
    printf("Packet send result: %d\n", res);
    recvSearchLibraryAck();
    return 0;
}

int testLibraryStoreTemplate(uint16_t pageID){
    uart_begin(PORT_NUM_2);
    int res = sendStoreTemplatePacket(0x01, pageID);
    printf("Packet send result: %d\n", res);
    recvGenerateTemplateAck();
    return 0;
}

int testLibraryLoadTemplate(uint8_t bufferID, uint16_t pageID){
    uart_begin(PORT_NUM_2);
    int res = sendLoadTemplatePacket(bufferID, pageID);
    printf("Packet send result: %d\n", res);
    recvLoadTemplateAck();
    return 0;
}

int testLibraryClearTemplates(){
    uart_begin(PORT_NUM_2);
    int res = sendClearLibraryPacket();
    printf("Packet send result %d\n", res);
    recvClearLibraryAck();
    return 0;
}

int testLibraryUploadFile(uint8_t bufferID){
    uart_begin(PORT_NUM_2);
    int res = sendUploadFilePacket(bufferID);
    printf("Packet send result %d\n", res);
    int resp = recvUploadFileAck();
    if(resp == 0){
        for(int j = 0; j < 2; j++){
            uint8_t* charFile = malloc(sizeof(uint8_t)*DATA_PKT_SIZE);
            if(charFile != NULL){
                recvUploadFile(&charFile);
                printf("File received:\n");
                for(int i = 0; i < DATA_PKT_SIZE; i++){
                    if((i%8) == 0 ){
                        printf("\n");
                    }
                    printf("0x%02x ", charFile[i]);
                }
                printf("\n");
            }
            free(charFile);
        }
    }
    return 0;
}

int sendHandshake(){
    uart_config_t config = {
        .baud_rate = BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB
    };
    int intr_alloc_flags = 0;
    ESP_ERROR_CHECK(uart_driver_install(PORT_NUM, BUF_SIZE*2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(PORT_NUM, &config));
    ESP_ERROR_CHECK(uart_set_pin(PORT_NUM, PIN_TX, PIN_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    uint8_t* data = (uint8_t*) malloc(13);

    typedef struct handshake_packet {
        uint16_t header;
        uint8_t addr[4];
        uint8_t  pid;
        uint16_t length;
        uint8_t  ins;
        uint8_t  ctrl;
        uint16_t checksum;
    } handshake_packet;

    handshake_packet* hs_pkt = malloc(sizeof(handshake_packet));
    hs_pkt->header = PKT_HEADER;
    hs_pkt->addr[0] = 0xFF;
    hs_pkt->addr[1] = 0xFF;
    hs_pkt->addr[2] = 0xFF;
    hs_pkt->addr[3] = 0xFF;
    hs_pkt->pid = PKT_PID_CMD;
    hs_pkt->length = 0x0004;
    hs_pkt->ins = 0x17;
    hs_pkt->ctrl = 0x00;
    hs_pkt->checksum = 0x001C;

    data[0] = (uint8_t) (hs_pkt->header >> 8);
    data[1] = (uint8_t) (hs_pkt->header & 0xFF);
    data[2] = hs_pkt->addr[0];
    data[3] = hs_pkt->addr[0];
    data[4] = hs_pkt->addr[0];
    data[5] = hs_pkt->addr[0];//addr[0] is same as all others, fix later
    data[6] = hs_pkt->pid;
    data[7] = (uint8_t)(hs_pkt->length >> 8);
    data[8] = (uint8_t)(hs_pkt->length & 0xFF);
    data[9] = hs_pkt->ins;
    data[10]= hs_pkt->ctrl;
    data[11]= (uint8_t)(hs_pkt->checksum >> 8);
    data[12]= (uint8_t)(hs_pkt->checksum & 0xFF);
    
    /*data[1] = 0x41;
    data[2] = 0x42;
    data[3] = 0x43;
    data[4] = 0x44;
    data[5] = 0x45;//addr[0] is same as all others, fix later
    data[6] = 0x46;
    data[7] = 0x47;
    data[8] = 0x48;
    data[9] = 0x49;
    data[10]= 0x4A;
    data[11]= 0x4B;
    data[12]= 0x4C;
    */
    //free(hs_pkt);
    uint8_t* recv = (uint8_t*)malloc(BUF_SIZE);
    for(int i = 0; i < BUF_SIZE; i++){
        recv[i] = 0x00;
    }
    printf("Testing fingerprint sensor handshake\n"); 
    printf("Sent: 0x%02x\n", (unsigned int)data[0]);
    printf("Sent: 0x%02x\n", (unsigned int)data[1]);
    printf("Sent: 0x%02x\n", (unsigned int)data[2]);
    printf("Sent: 0x%02x\n", (unsigned int)data[3]);
    printf("Sent: 0x%02x\n", (unsigned int)data[4]);
    printf("Sent: 0x%02x\n", (unsigned int)data[5]);
    printf("Sent: 0x%02x\n", (unsigned int)data[6]);
    printf("Sent: 0x%02x\n", (unsigned int)data[7]);
    printf("Sent: 0x%02x\n", (unsigned int)data[8]);
    printf("Sent: 0x%02x\n", (unsigned int)data[9]);
    printf("Sent: 0x%02x\n", (unsigned int)data[10]);
    printf("Sent: 0x%02x\n", (unsigned int)data[11]);
    printf("Sent: 0x%02x\n", (unsigned int)data[12]);
    uart_write_bytes(PORT_NUM, (const char *)data, 13);
    uart_wait_tx_done(PORT_NUM, 100);
    int buflen;
    while(buflen < 12){
        vTaskDelay(1);
        uart_get_buffered_data_len(PORT_NUM, (size_t*)&buflen);
        printf("%d\n", buflen);
    }
    uart_read_bytes(PORT_NUM, recv, BUF_SIZE, 20/portTICK_RATE_MS);
    printf("Received:\n");
    printf("Recv: 0x%02x\n", (unsigned int)recv[0]);
    printf("Recv: 0x%02x\n", (unsigned int)recv[1]);
    printf("Recv: 0x%02x\n", (unsigned int)recv[2]);
    printf("Recv: 0x%02x\n", (unsigned int)recv[3]);
    printf("Recv: 0x%02x\n", (unsigned int)recv[4]);
    printf("Recv: 0x%02x\n", (unsigned int)recv[5]);
    printf("Recv: 0x%02x\n", (unsigned int)recv[6]);
    printf("Recv: 0x%02x\n", (unsigned int)recv[7]);
    printf("Recv: 0x%02x\n", (unsigned int)recv[8]);
    printf("Recv: 0x%02x\n", (unsigned int)recv[9]);
    printf("Recv: 0x%02x\n", (unsigned int)recv[10]);
    printf("Recv: 0x%02x\n", (unsigned int)recv[11]);
    printf("Recv: 0x%02x\n", (unsigned int)recv[12]);
    return 0;
}
