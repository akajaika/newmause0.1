#include "driver/gpio.h"

void gpio_setup() {
    // 入力ピンの設定
    gpio_set_direction(GPIO_NUM_1, GPIO_MODE_INPUT);
    gpio_set_direction(GPIO_NUM_2, GPIO_MODE_INPUT);
    gpio_set_direction(GPIO_NUM_3, GPIO_MODE_INPUT);
    gpio_set_direction(GPIO_NUM_4, GPIO_MODE_INPUT);

    // 出力ピンの設定
    gpio_set_direction(GPIO_NUM_5, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_NUM_7, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_NUM_9, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_NUM_14, GPIO_MODE_OUTPUT);

}

void app_main() {
    gpio_setup();
}