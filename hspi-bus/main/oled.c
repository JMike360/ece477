#include "oled.h"
#include "hspi.h"

void display_upper(const char* str) {
    gpio_set_level(PIN_NUM_CS, 0);

    /* write into DR0 of HSPI's MOSI pin */

    gpio_set_level(PIN_NUM_CS, 1);
}

void display_lower(const char* str) {
    gpio_set_level(PIN_NUM_CS, 0);

    /* write into DR0 of HSPI's MOSI pin */

    gpio_set_level(PIN_NUM_CS, 1);
}