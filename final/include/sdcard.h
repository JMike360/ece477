#ifndef __SDCARD_H__
#define __SDCARD_H__

#include "driver/sdspi_host.h"
#include "driver/spi_common.h"
#include "esp_err.h"
#include "esp_log.h"

// Pin mapping when using HSPI.
#define PIN_NUM_MISO        GPIO_NUM_12
#define PIN_NUM_MOSI        GPIO_NUM_13
#define PIN_NUM_CLK         GPIO_NUM_27
#define PIN_NUM_CS_SD       GPIO_NUM_15

#define SPI_DMA_CHANNEL 1
#define SD_START            ((long)0x1 << 38)

#define MOUNT_POINT "/sdcard"

esp_err_t sdspiInit();
esp_err_t mountSD();
esp_err_t unmountSD();

#endif