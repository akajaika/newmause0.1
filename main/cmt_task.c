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
#include "MA732_read.h"

TaskHandle_t cmt_task_handle = NULL;

extern void int_cmt0(void);
extern void int_cmt1(void);
extern void int_cmt2(void);

// 通知されたら実処理するタスク
void cmt_task(void *arg) {
    while (true) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        // printf("Doing setup_cmt_timer1_0\n");
        int_cmt0();  // ここなら浮動小数点OK
        // printf("Doing setup_cmt_timer1_1\n");
        int_cmt1();
        // printf("Doing setup_cmt_timer1_2\n");
        int_cmt2();  // 同上

        if(enc_flag == true) {
            MA732_read();
            enc_flag = false;
        }
        if (imu_flag == true)
        {
            MPU6500_read_accel_gyro();
            imu_flag = false;
        }
        
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}