#include "hspi.h"

esp_err_t hspi_init(void) {
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };
    display_lower("");
    display_upper("Initializing SD card in SPI mode...");
    esp_err_t err = spi_bus_initialize(host.slot, &bus_cfg, SPI_DMA_CHANNEL);
    if (err == ESP_OK)
        display_upper("SPI bus init success");
    else
        display_upper("SPI bus init failed (%s).", esp_err_to_name(err));
    return err;
}