#include "include/hspi.h"
#include <string.h>
#include "driver/gpio.h"

#define TAG "HSPI"

esp_err_t hspi_init() {
    // ensure module init once
    static int isInit = 0;
    if(isInit == 1) {
        ESP_LOGI(TAG, "Already initialized. Aborting attempt.");
        return ESP_OK;
    }

    // default config for SD interface
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4094,
    };

    // initialize HSPI bus
    esp_err_t err = spi_bus_initialize(HSPI_HOST, &bus_cfg, SPI_DMA_CHANNEL);
    if (err == ESP_OK) {
        isInit = 1;
        ESP_LOGI(TAG, "Successfully initialized bus.");
    }
    else
        ESP_LOGE(TAG, "Failed to init bus (%s).", esp_err_to_name(err));
    
    return err;
}

esp_err_t hspi_transmit(spi_device_handle_t dev, uint32_t msg, int len) {
    uint16_t data = SPI_SWAP_DATA_TX(msg, len);
    spi_transaction_t t;
    
    memset(&t, '\0', sizeof(t));
    t.length = len;
    t.tx_buffer = &data;

    esp_err_t err = spi_device_polling_transmit(dev, &t);
    if (err == ESP_OK)
        ESP_LOGI(TAG, "Successfully transmitted 0x%08x, returning 0x%02x.",
            msg, (uint32_t)t.rx_buffer);
    else
        ESP_LOGE(TAG, "Failed to transmit 0x%x (%s).",
            data, esp_err_to_name(err));

    return err;
}
