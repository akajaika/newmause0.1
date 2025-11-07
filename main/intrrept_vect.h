#include "esp_intr_types.h"
#include "parameters.h"
#include "glob_ver.h"
#include "static_parameters.h"
#include "typedef.h"
#include "driver/timer.h"
#include "soc/timer_group_struct.h"
#include <stdlib.h> 
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern TaskHandle_t cmt_task_handle;

// extern void int_cmt0(void);

static void IRAM_ATTR cmt_isr(void* arg) {
    // ここに元の intr_cmt0() の中身を書く
    TIMERG0.hw_timer[TIMER_0].update.val = 1;                     // タイマレジスタ更新
    TIMERG0.int_clr_timers.t0_int_clr = 1;                            // 割り込みフラグクリア
    TIMERG0.hw_timer[TIMER_0].config.tn_alarm_en = TIMER_ALARM_EN; // 次回アラーム有効化

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // // --- 制御用のタイムスタンプ更新や制御演算 ---
    // int_cmt0();   // PID計算・PWM出力更新など短時間で完了するもの　浮動小数でダメだった
    
    // --- センサ読み取り要求フラグだけ立てる ---
    enc_flag = true;
    imu_flag = true;    

    if (cmt_task_handle != NULL) {
        xTaskNotifyFromISR(cmt_task_handle, 0, eNoAction, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}
