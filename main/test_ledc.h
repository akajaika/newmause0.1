#include "driver/ledc.h"

extern void ledc_test_pwm_init(gpio_num_t pin_l, gpio_num_t pin_r, int duty8);
extern void ledc_test_pwm2(gpio_num_t pin, int duty8);
extern void update_duty(int duty_r8, int duty_l8); 