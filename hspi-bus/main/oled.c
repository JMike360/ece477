#include "include/oled.h"
#include "include/hspi.h"
#include <unistd.h>
#include <string.h>

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
        .dummy_bits = 16,
        .mode = 0,
        .duty_cycle_pos = 128,
        .cs_ena_pretrans = 8,
        .cs_ena_posttrans = 8,
        .clock_speed_hz = 10,
        .input_delay_ns = 0,
        .queue_size = 4,
        .spics_io_num = PIN_NUM_CS_OLED,
        .pre_cb = 1,
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

void clear_all() {
    // declared static to allow access by device even after function exited
    static spi_transaction_t spi_trans;
    memset(&trans, '\0', sizeof(spi_transaction_t));

    spi_trans.length = 10;
    spi_trans.tx_data[0] = (OLED_CMD | OLED_CLEAR_DISP) >> 2;
    spi_trans.tx_data[1] = (OLED_CMD | OLED_CLEAR_DISP) << 8;
    spi_trans.flags = SPI_TRANS_USE_TXDATA;

    esp_err_t err = spi_device_polling_transmit(oled_handle, &spi_trans);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to queue OLED transaction (%s).", esp_err_to_name(err));
        return;
    }
    spi_transaction_t* spi_trans_ptr = &spi_trans;
    err = spi_device_get_trans_result(oled_handle, &spi_trans_ptr, portMAX_DELAY);
    if (err != ESP_OK)
        ESP_LOGE(TAG, "Failed to get OLED result (%s).", esp_err_to_name(err));
    else
        ESP_LOGI(TAG, "Successfully cleared OLED");
}

void display_upper(const char* str) {
    spi_transaction_t trans;
    int i = 0;
    while(str[i] != '\0') {
        trans.tx_data[0] = OLED_DATA | str[i++];
        spi_device_queue_trans(oled_handle, &trans, portMAX_DELAY);
    }
    trans.tx_data[0] = OLED_DATA | '\0';
    for(int j = i; j < 16; j++) {
        spi_device_queue_trans(oled_handle, &trans, portMAX_DELAY);
    }
}

void display_lower(const char* str) {
    
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