#include "include/hspi.h"

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