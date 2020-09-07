#ifndef __HSPI_H__
#define __HSPI_H__

#include "driver/sdspi_host.h"
#include "driver/spi_common.h"
#include <unistd.h>
#include "esp_err.h"
#include "esp_log.h"

// Pin mapping when using HSPI.
#define PIN_NUM_MISO        12
#define PIN_NUM_MOSI        13
#define PIN_NUM_CLK         14
#define PIN_NUM_CS_OLED     15
#define PIN_NUM_CS_SD       32

// alias for HSPI
#define HSPI_HOST SPI2_HOST

#define SPI_DMA_CHANNEL 1

esp_err_t hspi_init();

#endif