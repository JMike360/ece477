#include "include/hspi.h"
#include <string.h>

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

    spi_trans.length = len;

    msg <<= (32-len);
    spi_trans.tx_data[0] = (spi_trans.tx_data[0] >> 24) & 0xFF;
    spi_trans.tx_data[1] = (spi_trans.tx_data[1] >> 16) & 0xFF;
    spi_trans.tx_data[2] = (spi_trans.tx_data[2] >> 8) & 0xFF;
    spi_trans.tx_data[3] = (spi_trans.tx_data[3] >> 0) & 0xFF;

    spi_trans.user = (void*)0;
    spi_trans.flags = SPI_TRANS_USE_TXDATA;

    esp_err_t err = spi_device_polling_transmit(dev, &spi_trans);
    if (err == ESP_OK)
        ESP_LOGI(TAG, "Successfully transmitted cmd 0x%x.", msg);
    else
        ESP_LOGE(TAG, "Failed to transmit cmd 0x%x (%s).", msg, esp_err_to_name(err));
    
    return err;
}