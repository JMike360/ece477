#include "include/hspi.h"
#include <string.h>
#include "driver/gpio.h"

#define TAG "HSPI"

esp_err_t hspi_init() {
    // ensure module init once
    static int isInit = 0;
    if(isInit == 1) {
        ESP_LOGI(TAG, "HSPI already initialized. Aborting attempt.");
        return ESP_OK;
    }

    // default config for SD interface
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };

    // initialize HSPI bus
    esp_err_t err = spi_bus_initialize(HSPI_HOST, &bus_cfg, SPI_DMA_CHANNEL);
    if (err == ESP_OK) {
        isInit = 1;
        ESP_LOGI(TAG, "Successfully initialized SPI bus.");
    }
    else
        ESP_LOGE(TAG, "Failed to init SPI bus (%s).", esp_err_to_name(err));
    
    return err;
}

esp_err_t hspi_transmit(spi_device_handle_t dev, uint32_t msg, int len) {
    spi_transaction_t spi_trans;
    memset(&spi_trans, '\0', sizeof(spi_trans));

    // round msg len up to nearest multiple of 8 bits
    spi_trans.length = ( (len+7) / 8 ) * 8;

    msg <<= 32 - spi_trans.length;

    spi_trans.tx_data[0] = (msg >> 24) & 0xFF;
    spi_trans.tx_data[1] = (msg >> 16) & 0xFF;
    spi_trans.tx_data[2] = (msg >> 8) & 0xFF;
    spi_trans.tx_data[3] = (msg >> 0) & 0xFF;

    spi_trans.user = (void*)0;
    spi_trans.flags = SPI_TRANS_USE_TXDATA;

    esp_err_t err = spi_device_polling_transmit(dev, &spi_trans);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Successfully transmitted 0x%02x 0x%02x 0x%02x 0x%02x, returning 0x%02x.",
            spi_trans.tx_data[0], spi_trans.tx_data[1], spi_trans.tx_data[2],
            spi_trans.tx_data[3], (uint32_t)spi_trans.rx_buffer);
    }
    else
        ESP_LOGE(TAG, "Failed to transmit 0x%02x 0x%02x 0x%02x 0x%02x (%s).",
            spi_trans.tx_data[0], spi_trans.tx_data[1], spi_trans.tx_data[2],
            spi_trans.tx_data[3], esp_err_to_name(err));
    
    return err;
}