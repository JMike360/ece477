#include "hspi.h"
#include "esp_err.h"
#include "esp_log.h"

esp_err_t hspi_init(void) {
    // ensure module init once
    static int isInit = 0;
    if(isInit == 1)
        return;

    // default config for SD interface
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
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
    if (err == ESP_OK)
        isInit = 1;
    else
        ESP_LOGE("Failed to init SPI bus (%s).", esp_err_to_name(err));
    
    return err;
}