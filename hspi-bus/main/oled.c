#include "include/oled.h"
#include "include/hspi.h"
#include <unistd.h>

static spi_device_handle_t oled_handle;

esp_err_t oled_init() {
    // ensure module init once
    static int isInit = 0;
    if(isInit == 1) {
        ESP_LOGI(TAG, "OLED already registered. Aborting attempt.");
        return ESP_OK;
    }

    // config OLED communication
    spi_device_interface_config_t dev_cfg = {
        .mode = 0,
        .clock_speed_hz = 10*1000*1000,
        .queue_size = 4,
        .spics_io_num = PIN_NUM_CS_OLED,
    };

    // register OLED as HSPI slave device
    esp_err_t err = spi_bus_add_device(HSPI_HOST, &dev_cfg, &oled_handle);
    if (err == ESP_OK) {
        isInit = 1;
        ESP_LOGI(TAG, "Successfully registered OLED.");
    }
    else
        ESP_LOGE(TAG, "Failed to register OLED (%s).", esp_err_to_name(err));

    return err;
}

esp_err_t clear_all() {
    return hspi_transmit(oled_handle, OLED_CMD | OLED_CLEAR_DISP, OLED_NUM_BITS);
}

esp_err_t display_upper(const char* str) {
    int i = 0;
    esp_err_t err = ESP_OK;

    hspi_transmit(oled_handle, OLED_CMD | OLED_CURSOR_RET, OLED_NUM_BITS);
    while( (str[i] != '\0') && (i < 16) && (err == ESP_OK)) {
        err = hspi_transmit(oled_handle, OLED_DATA | str[i++], OLED_NUM_BITS);
    }
    hspi_transmit(oled_handle, OLED_CMD | OLED_CURSOR_RET, OLED_NUM_BITS);

    return err;
}

esp_err_t display_lower(const char* str) {
    return ESP_OK;
}

void app_main() {
    hspi_init();
    sleep(1);
    oled_init();
    sleep(1);
    clear_all();
    sleep(1);
    display_upper("this is a test");
}