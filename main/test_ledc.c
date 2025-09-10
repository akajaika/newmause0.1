#include "driver/ledc.h"

void ledc_test_pwm_init(gpio_num_t pin_r, gpio_num_t pin_l, int duty8) {
    ledc_timer_config_t tcfg = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 2000,
        .duty_resolution = LEDC_TIMER_10_BIT,
    };
    ledc_timer_config(&tcfg);

    ledc_channel_config_t ccfg_l = {
        .gpio_num = pin_l,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = duty8,
        .hpoint = 0
    };
    ledc_channel_config(&ccfg_l);

        ledc_channel_config_t ccfg_r = {
        .gpio_num = pin_r,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_1,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = duty8,
        .hpoint = 0
    };
    ledc_channel_config(&ccfg_r);
}

void ledc_test_pwm2(gpio_num_t pin, int duty8) {
    ledc_timer_config_t tcfg = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_1,
        .freq_hz = 2000,
        .duty_resolution = LEDC_TIMER_14_BIT,
    };
    ledc_timer_config(&tcfg);

    ledc_channel_config_t ccfg = {
        .gpio_num = pin,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_2,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_2,
        .duty = duty8,
        .hpoint = 0
    };
    ledc_channel_config(&ccfg);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, duty8);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
}

void update_duty(int duty_r8, int duty_l8) {
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, duty_r8);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);

    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty_l8);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    printf("duty_r8: %d, duty_l8: %d\n", duty_r8, duty_l8);
}