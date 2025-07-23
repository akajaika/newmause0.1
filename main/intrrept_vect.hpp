#include "esp_intr_types.h"
#include "parameters.hpp"
#include "glob_ver.hpp"
#include "static_parameters.hpp"
#include "typedef.hpp"
#include "motor.hpp"
#include "MA732.hpp"

extern "C" {
    #include "driver/timer.h"              // TIMERG0 や timer_xxx API のプロトタイプ
    #include "soc/timer_group_struct.h"    // TIMERG0 構造体定義
}

extern void intr_cmt0(void);
extern void intr_cmt2(void);

static void IRAM_ATTR cmt_isr(void* arg) {
    // ここに元の intr_cmt0() の中身を書く
    TIMERG0.hw_timer[TIMER_0].update.tn_update = 1;                     // タイマレジスタ更新
    TIMERG0.int_clr_timers.wdt_int_clr = 1;                            // 割り込みフラグクリア
    TIMERG0.hw_timer[TIMER_0].config.tn_alarm_en = TIMER_ALARM_EN; // 次回アラーム有効化
    intr_cmt0();
    intr_cmt2();
}


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
    timer_isr_register(TIMER_GROUP_0, TIMER_0, cmt_isr,
                       nullptr, ESP_INTR_FLAG_IRAM, nullptr);
    timer_start(TIMER_GROUP_0, TIMER_0);
}