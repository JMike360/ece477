/**************************************************
* To do list:
* i. replace every logging function with OLED display
* ii. add function documentation
*
**************************************************/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "sdkconfig.h"
#include "../include/sdcard.h"
#include "driver/gpio.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include <string.h>

#define TAG "SD-CARD"

sdmmc_card_t* sdcard = NULL;
static uint8_t spi_init_flag = 0x0;

/**************************************************
 * spiInit
 * Initialize SPI2 (or HSPI) for connection using
 * PIN_NUM_MOSI, PIN_NUM_MISO, PIN_NUM_CLK.
 * 
 * input:
 * void
 * 
 * output:
 * esp_err_t - return status
**************************************************/
esp_err_t spiInit() {
    // ensure module init once
    if ( ( spi_init_flag & (1 << SPI2_HOST) ) != 0 ) {
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
    esp_err_t err = spi_bus_initialize(SPI2_HOST, &bus_cfg, SPI_DMA_CHANNEL);
    if (err == ESP_OK) {
        spi_init_flag |= (1 << SPI2_HOST);
        ESP_LOGI(TAG, "Successfully initialized bus.");
    }
    else
        ESP_LOGE(TAG, "Failed to initialize bus (%s).", esp_err_to_name(err));
    
    return err;
}

/**************************************************
 * mount_fs
 * Mount SD card by initializing it through SPI.
 * Prior to calling this function, spiInit() should be
 * called to initialize the SPI. SD card information
 * is stored in 'sdcard' in sdcard.c
 * 
 * input:
 * void
 * 
 * output:
 * esp_err_t - return status
**************************************************/
esp_err_t mount_fs() {
    esp_vfs_fat_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 4,
        .allocation_unit_size = 8 * 1024,
    };

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    sdspi_device_config_t dev_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    dev_config.gpio_cs = PIN_NUM_CS_SD;
    dev_config.host_id = host.slot;

    sleep(3);

    esp_err_t ret = esp_vfs_fat_sdspi_mount(MOUNT_POINT, &host, &dev_config,
        &mount_config, &sdcard);
    if (ret == ESP_OK)
        ESP_LOGI(TAG, "Successfully mounted SD card.");
    else
        ESP_LOGE(TAG, "Failed to mount SD card (%s).", esp_err_to_name(ret));
    
    return ret;
}

/**************************************************
 * umount_fs
 * Unmount the SD card that was previously mounted
 * using mount_fs() and stored as 'sdcard' in sdcard.c
 * 
 * input:
 * void
 * 
 * output:
 * esp_err_t - return status
**************************************************/
esp_err_t umount_fs() {
    esp_err_t ret = esp_vfs_fat_sdcard_unmount(MOUNT_POINT, sdcard);
    if (ret == ESP_OK)
        ESP_LOGI(TAG, "Successfully unmounted SD card.");
    else
        ESP_LOGE(TAG, "Failed to unmount SD card (%s).", esp_err_to_name(ret));
    return ret;
}