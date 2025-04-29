#include "driver/gptimer.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "driver/spi_master.h"  
#include "MPU6500.hpp"
#include "PCA9533.hpp"
#include <stdio.h>

bool timer_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx){
    spi_device_polling_transmit(dev_handle, &trans);
    return true; 
}

void init_gptimer(){
    gptimer_config_t gptimer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000, // 1MHz
        .intr_priority = 0,
        .flags = {
            .intr_shared = false,
            .allow_pd = true,
            .backup_before_sleep = false,
        },   
    };

    gptimer_handle_t gptimer;

    gptimer_new_timer(&gptimer_config, &gptimer);

    gptimer_alarm_config_t alarm_config = {
        .alarm_count = 1000, // resolution_hz = 1 000 000（1 tick = 1 µs)
        .reload_count = 0,
        .flags = {
            .auto_reload_on_alarm = true,
        },
    };
    gptimer_set_alarm_action(gptimer, &alarm_config);

    gptimer_event_callbacks_t cbs = {
        .on_alarm = timer_callback, 
    };
    gptimer_register_event_callbacks(gptimer, &cbs, NULL);

    gptimer_enable(gptimer);
    gptimer_start(gptimer);

}

void app_main(){
    init_gptimer();
}