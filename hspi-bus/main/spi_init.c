#include "include/spi_init.h"
#include <string.h>
#include "driver/gpio.h"

#define TAG "HSPI"

static uint8_t spi_init_flag = 0x0;

esp_err_t spi_init(spi_host_device_t spi_host) {
    // ensure module init once
    if ( ( spi_init_flag & (1 << spi_host) ) != 0 ) {
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
        .max_transfer_sz = 4000,
    };

    // initialize HSPI bus
    esp_err_t err = spi_bus_initialize(spi_host, &bus_cfg, SPI_DMA_CHANNEL);
    if (err == ESP_OK) {
        spi_init_flag |= (1 << spi_host);
        ESP_LOGI(TAG, "Successfully initialized bus.");
    }
    else
        ESP_LOGE(TAG, "Failed to initialize bus (%s).", esp_err_to_name(err));
    
    return err;
}

uint32_t spi_transmit(spi_device_handle_t dev, uint32_t msg, int len) {
    uint16_t data = SPI_SWAP_DATA_TX(msg, len);
    spi_transaction_t t;
    uint32_t rec_buff;
    
    memset(&t, '\0', sizeof(t));
    t.length = len;
    t.tx_buffer = &data;
    t.rx_buffer = &rec_buff;

    esp_err_t err = spi_device_polling_transmit(dev, &t);
    if (err == ESP_OK)
        ESP_LOGI(TAG, "Successfully transmitted 0x%08x, returning 0x%02x.",
            msg, (uint32_t)t.rx_buffer);
    else
        ESP_LOGE(TAG, "Failed to transmit 0x%x (%s).",
            data, esp_err_to_name(err));

    return rec_buff;
}
