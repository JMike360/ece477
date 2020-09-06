#include "oled.h"
#include "hspi.h"

static spi_device_handle_t* oled_ptr;

void oled_init() {
    // ensure module init once
    static int isInit = 0;
    if(isInit == 1)
        return;

    // config OLED communication
    spi_device_interface_config_t dev_cfg = {
        .command_bits = 10,
        .address_bits = 32,
        .dummy_bits = 16,
        .mode = 0,
        .duty_cycle_pos = 128,
        .cs_ena_pretrans = 8,
        .cs_ena_posttrans = 8,
        .clock_speed_hz = 10,
        .input_delay_ns = 0,
        .spics_io_num = PIN_NUM_CS_OLED,
        .flags = 0,
        .queue_size = 4,
        .pre_cb = SPI_TRANS_VARIABLE_CMD,
        .post_cb = SPI_TRANS_VARIABLE_CMD,
    };

    // register OLED as HSPI slave device
    err = spi_bus_add_device(HSPI_HOST, dev_cfg, oled_ptr);
    if (err == ESP_OK)
        isInit = 1;
    else
        ESP_LOGE(TAG, "Failed to init SPI bus (%s).", esp_err_to_name(err));

    return err;
}

void clear_all() {
    spi_transaction_t trans = {
        .cmd = OLED_CMD | OLED_CLEAR_DISP,
    };
    esp_err_t err = spi_device_queue_trans(oled_ptr, &trans, portMAX_DELAY);
    if (err != ESP_OK)
        ESP_LOGE(TAG, "Failed to queue OLED transaction (%s).", esp_err_to_name(err));
        return;
    spi_transaction_t* trans_ptr = &trans;
    err = spi_device_get_trans_result(oled_ptr, &trans_ptr, portMAX_DELAY);
    if (err != ESP_OK)
        ESP_LOGE(TAG, "Failed to get OLED result (%s).", esp_err_to_name(err));
    ESP_LOGI(TAG, "Successfully cleared OLED");
}

void display_upper(const char* str) {
    spi_transaction_t trans;
    int i = 0;
    trans.cmd = OLED_CMD | OLED_
    while(str[i] != '\0') {
        trans.cmd = OLED_DATA | str[i++];
        spi_device_queue_trans(oled_ptr, &trans, portMAX_DELAY);
    }
    trans.cmd = OLED_DATA | '\0';
    for(int j = i; j < 16; j++) {
        spi_device_queue_trans(oled_ptr, &trans, portMAX_DELAY);
    }
}

void display_lower(const char* str) {
    
}