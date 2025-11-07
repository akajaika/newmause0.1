#include "esp_intr_types.h"
#include "parameters.h"
#include "glob_ver.h"
#include "static_parameters.h"
#include "typedef.h"
#include "driver/timer.h"
#include "soc/timer_group_struct.h"
#include <stdlib.h>
#include "intrrept_vect.h"
#include "cmt_task.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern TaskHandle_t cmt_task_handle;

void setup_cmt_timer() {
    // タイマ初期化：TIMERG0, TIMER_0, カウントアップ, 80MHz/80 = 1MHz
    timer_config_t cfg = {
        .alarm_en = TIMER_ALARM_EN,
        .counter_en = TIMER_START,
        .intr_type = TIMER_INTR_LEVEL,
        .counter_dir = TIMER_COUNT_UP,
        .auto_reload = TIMER_AUTORELOAD_EN,
        .divider = 80
    };
    timer_init(TIMER_GROUP_0, TIMER_0, &cfg);

    // 1 MHz カウント → 1 ms ごとに 1000 カウント
    timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 1000);
    timer_enable_intr(TIMER_GROUP_0, TIMER_0);
    esp_err_t err = timer_isr_register(
        TIMER_GROUP_0, TIMER_0, cmt_isr,
        NULL, ESP_INTR_FLAG_IRAM, NULL
    );

    xTaskCreatePinnedToCore(cmt_task, "cmt_task", 4096, NULL, 10, &cmt_task_handle, 1);

    timer_start(TIMER_GROUP_0, TIMER_0);
    printf("Doing setup_cmt_timer2\n");
}