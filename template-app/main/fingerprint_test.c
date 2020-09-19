
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
#define BAUD_RATE 115200
#define PORT_NUM 2
#define STACK_SIZE 2048
#define BUF_SIZE 1024

#include "driver/uart.h"

int sendHeader(){
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

    uint8_t* data = (uint8_t*) malloc(BUF_SIZE);

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
    free(hs_pkt);
    printf("Testing fingerprint sensor handshake\n"); 
    printf("Sent: %x\n", (unsigned int)data);
    uart_write_bytes(PORT_NUM, (const char *)data, 13);
    uart_wait_tx_done(PORT_NUM, 100);
    int buflen;
    while(buflen < 12){
        vTaskDelay(1);
        uart_get_buffered_data_len(PORT_NUM, (size_t*)&buflen);
        printf("%d\n", buflen);
    }
    uart_read_bytes(PORT_NUM, data, BUF_SIZE, 20/portTICK_RATE_MS);
    printf("Received: %x\n", (unsigned int)data);
    return 0;
}