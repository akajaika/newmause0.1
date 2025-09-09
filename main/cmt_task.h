#ifndef CMT_TASK_H
#define CMT_TASK_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern TaskHandle_t cmt_task_handle;

void cmt_task(void);

#endif