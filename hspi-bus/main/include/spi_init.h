#ifndef __HSPI_H__
#define __HSPI_H__

#include "driver/sdspi_host.h"
#include "driver/spi_common.h"
#include <stdint.h>
#include "esp_err.h"
#include "esp_log.h"

// Pin mapping when using HSPI.
#define PIN_NUM_MISO        GPIO_NUM_12
#define PIN_NUM_MOSI        GPIO_NUM_13
#define PIN_NUM_CLK         GPIO_NUM_14
#define PIN_NUM_CS_SD       GPIO_NUM_15

#define SPI_DMA_CHANNEL 1

esp_err_t spi_init(spi_host_device_t spi_host);
uint32_t spi_transmit(spi_device_handle_t dev, uint32_t msg, int len);

#endif