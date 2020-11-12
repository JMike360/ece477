#include "uart_setup.h"
void uart_begin(int port){
    switch(port){
        case PORT_NUM_0:
            if(!uart_is_driver_installed(PORT_NUM_0)){
                uart_config_t config = {
                    .baud_rate = BAUD_RATE_IO,
                    .data_bits = UART_DATA_8_BITS,
                    .parity = UART_PARITY_DISABLE,
                    .stop_bits = UART_STOP_BITS_1,
                    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
                    .source_clk = UART_SCLK_APB
                };
                int intr_alloc_flags = 0;
                ESP_ERROR_CHECK(uart_driver_install(PORT_NUM_0, BUF_SIZE*2, 0, 0, NULL, intr_alloc_flags));
                ESP_ERROR_CHECK(uart_param_config(PORT_NUM_0, &config));
                ESP_ERROR_CHECK(uart_set_pin(PORT_NUM_0, PIN_TX_0, PIN_RX_0, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
            }
            break;
        case PORT_NUM_1:
            if(!uart_is_driver_installed(PORT_NUM_1)){
                uart_config_t config = {
                    .baud_rate = BAUD_RATE_IO,
                    .data_bits = UART_DATA_8_BITS,
                    .parity = UART_PARITY_DISABLE,
                    .stop_bits = UART_STOP_BITS_1,
                    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
                    .source_clk = UART_SCLK_APB
                };
                int intr_alloc_flags = 0;
                ESP_ERROR_CHECK(uart_driver_install(PORT_NUM_1, BUF_SIZE*2, 0, 0, NULL, intr_alloc_flags));
                ESP_ERROR_CHECK(uart_param_config(PORT_NUM_1, &config));
                ESP_ERROR_CHECK(uart_set_pin(PORT_NUM_1, PIN_TX_1, PIN_RX_1, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
            }
            break;
        case PORT_NUM_2:
            if(!uart_is_driver_installed(PORT_NUM_2)){
                uart_config_t config = {
                    .baud_rate = BAUD_RATE,
                    .data_bits = UART_DATA_8_BITS,
                    .parity = UART_PARITY_DISABLE,
                    .stop_bits = UART_STOP_BITS_1,
                    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
                    .source_clk = UART_SCLK_APB
                };
                int intr_alloc_flags = 0;
                ESP_ERROR_CHECK(uart_driver_install(PORT_NUM_2, BUF_SIZE*2, 0, 0, NULL, intr_alloc_flags));
                ESP_ERROR_CHECK(uart_param_config(PORT_NUM_2, &config));
                ESP_ERROR_CHECK(uart_set_pin(PORT_NUM_2, PIN_TX_2, PIN_RX_2, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
            }
            break;
        default:
            break;
    }
}

void uart_end(int port){
    ESP_ERROR_CHECK(uart_driver_delete(port));
}