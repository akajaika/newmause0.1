#include "test.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// C の呼び出し規約でシンボルをエクスポート
extern "C" void app_main() {
    // 必ずタスク遅延を入れないとウォッチドッグに引っかかります
    while (true) {
        test_run();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}