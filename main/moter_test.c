#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "esp_adc/adc_continuous.h"
#include "esp_log.h"
#include "esp_err.h"
#include <string.h>
#include "driver/mcpwm_timer.h"
#include "driver/mcpwm_oper.h"
#include "driver/mcpwm_gen.h"
#include "driver/mcpwm_cmpr.h"
#include "driver/gpio.h"
#include "parameters.h"
#include "glob_ver.h"
#include "static_parameters.h"
#include "typedef.h"

#define MCPWM_GEN_COMPARE_EVENT_ACTION_COUNT 1
#define GEN_GPIO_NUM_r 38
#define GEN_GPIO_NUM_l 17

#define NUM_TIMER_ACTIONS 1

#define duty_r_ticks 0 // デューティサイクルの初期値 (0-500)
#define duty_l_ticks 0 // デューティサイクルの初期値 (0-500)
#define new_duty_r 0 // 新しいデューティサイクル (0-500)
#define new_duty_l 0 // 新しいデューティサイクル (0-500)

t_motor motor;

esp_err_t err;

esp_err_t e;

mcpwm_timer_handle_t m_timer = NULL;
mcpwm_oper_handle_t oper = NULL;
mcpwm_gen_handle_t gen_r,gen_l = NULL;

void setup_motor(){

    motor.motor_r.duty = 0; // 初期デューティサイクル
    motor.motor_l.duty = 0; // 初期デューティサイクル

    // PH 出力 (方向) 割り当て
    gpio_set_direction(GPIO_NUM_33, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_NUM_18, GPIO_MODE_OUTPUT);
    // MODE 出力
    gpio_set_direction(GPIO_NUM_21, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_NUM_21, 1); // PHASE/ENABLE モード選択

    //タイマ初期化
    mcpwm_timer_config_t mcpwm_new_timer_cfg = {
        .group_id = 0,
        .clk_src =  MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = 10000000,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
        .period_ticks = 1000, // 1ms ごとに割り込み
        .intr_priority = 0, 
        .flags = {
            .update_period_on_empty = false,
            .update_period_on_sync = false,
        },
    };

    ESP_LOGI("MOTOR", "Timer period_ticks = %" PRIu32, mcpwm_new_timer_cfg.period_ticks);

    err = mcpwm_new_timer(&mcpwm_new_timer_cfg, &m_timer);
    if (err != ESP_OK) {
        ESP_LOGE("MOTOR", "Failed to create timer: %s", esp_err_to_name(err));
        return;
    }


    //オペレーター初期化
    mcpwm_operator_config_t mcpwm_new_operator_cfg = {
        .group_id = 0,
        .intr_priority = 0,
    };
    err = mcpwm_new_operator(&mcpwm_new_operator_cfg, &oper);
    if (err != ESP_OK) {
        ESP_LOGE("MOTOR", "Failed to create operator: %s", esp_err_to_name(err));
        return;
    }


    //timer,oper接続
    err = mcpwm_operator_connect_timer(oper, m_timer);
    if (err != ESP_OK) {
        ESP_LOGE("MOTOR", "Failed to connect operator and timer: %s", esp_err_to_name(err));
        return;
    }

    //コンパレーター初期化
    mcpwm_comparator_config_t mcpwm_new_comparator_cfg_r = {
        .intr_priority = 0,
        .flags.update_cmp_on_tez = true, 
    };
    mcpwm_comparator_config_t mcpwm_new_comparator_cfg_l = {
        .intr_priority = 0,
        .flags.update_cmp_on_tez = true, 
    };

    err = mcpwm_new_comparator(oper, &mcpwm_new_comparator_cfg_r, &cmpr_r);
    if (err != ESP_OK) {
        ESP_LOGE("MOTOR", "Failed to create comparator R: %s", esp_err_to_name(err));
        return;
    }
    err = mcpwm_new_comparator(oper, &mcpwm_new_comparator_cfg_l, &cmpr_l);
    if (err != ESP_OK) {
        ESP_LOGE("MOTOR", "Failed to create comparator R: %s", esp_err_to_name(err));
        return;
    }

    //ジェネレーター初期化
    mcpwm_generator_config_t mcpwm_new_generator_cfg_r = {
        .gen_gpio_num = GEN_GPIO_NUM_r
    };
    mcpwm_generator_config_t mcpwm_new_generator_cfg_l = {
        .gen_gpio_num = GEN_GPIO_NUM_l
    };
    mcpwm_gen_timer_event_action_t gen_timer_action = {
        .direction = MCPWM_TIMER_DIRECTION_UP,
        .event = MCPWM_TIMER_EVENT_EMPTY,
        .action = MCPWM_GEN_ACTION_HIGH,
    };

    err = mcpwm_new_generator(oper, &mcpwm_new_generator_cfg_r, &gen_r);
    if (err != ESP_OK) {
        ESP_LOGE("MOTOR", "Failed to create generator R: %s", esp_err_to_name(err));
        return;
    }
    err = mcpwm_new_generator(oper, &mcpwm_new_generator_cfg_l, &gen_l);
    if (err != ESP_OK) {
        ESP_LOGE("MOTOR", "Failed to create generator R: %s", esp_err_to_name(err));
        return;
    }

    mcpwm_gen_compare_event_action_t gen_cmpr_action_r = {
        .direction = MCPWM_TIMER_DIRECTION_UP,
        .comparator = cmpr_r,
        .action = MCPWM_GEN_ACTION_LOW,
    };
    mcpwm_gen_compare_event_action_t gen_cmpr_action_l = {
        .direction = MCPWM_TIMER_DIRECTION_UP,
        .comparator = cmpr_l,
        .action = MCPWM_GEN_ACTION_LOW,
    };

    ESP_LOGI("MOTOR", "setup_motor completebefore3");

    if (gen_r == NULL) {
        ESP_LOGE("MOTOR", "gen_r is NULL!");
    } else {
        ESP_LOGI("MOTOR", "gen_r is valid: %p", gen_r);
    }

    if (gen_l == NULL) {
        ESP_LOGE("MOTOR", "gen_l is NULL!");
    } else {
        ESP_LOGI("MOTOR", "gen_l is valid: %p", gen_l);
    }

    ESP_LOGI("MOTOR", "setup_motor completebefore2");

    mcpwm_generator_set_actions_on_compare_event(gen_r,gen_cmpr_action_r,MCPWM_GEN_COMPARE_EVENT_ACTION_END());
    mcpwm_generator_set_actions_on_compare_event(gen_l,gen_cmpr_action_l,MCPWM_GEN_COMPARE_EVENT_ACTION_END() );

    ESP_LOGI("MOTOR", "setup_motor completebefore2");

    ESP_LOGI("MOTOR", "about to set timer event on gen_r = %p", gen_r);
    err = mcpwm_generator_set_actions_on_timer_event(gen_r,gen_timer_action);
    if (err != ESP_OK) {
        ESP_LOGE("MOTOR", "Failed to set_actions_on_timer_event R: %s", esp_err_to_name(err));
        return;
    }
    
    ESP_LOGI("MOTOR", "about to set timer event on gen_r = %p", gen_l);
    err = mcpwm_generator_set_actions_on_timer_event(gen_l,gen_timer_action);
    if (err != ESP_OK) {
        ESP_LOGE("MOTOR", "Failed to set_actions_on_timer_event L: %s", esp_err_to_name(err));
        return;
    }

    ESP_LOGI("MOTOR", "setup_motor completebefore");

    mcpwm_timer_enable(m_timer);

    ESP_LOGI("MOTOR", "setup_motor completebefore_timer_enable\n\n");
    // mcpwm_comparator_set_compare_value(cmpr_r, 0);
    // mcpwm_comparator_set_compare_value(cmpr_l, 0); 

    ESP_LOGI("MOTOR", "setup_motor complete"); 
}

void mot_on(short duty_r, short duty_l) {
    if (cmpr_r == NULL) ESP_LOGE("MOTOR","cmpr_r is NULL!");
    if (cmpr_l == NULL) ESP_LOGE("MOTOR","cmpr_l is NULL!");
    if (duty_r >= 500) duty_r = 499;
    if (duty_l >= 500) duty_l = 499;
    e = mcpwm_comparator_set_compare_value(cmpr_r, duty_r);
    ESP_LOGI("MOTOR","set cmpr_r -> %d (err=%s)", duty_r, esp_err_to_name(e));
    e = mcpwm_comparator_set_compare_value(cmpr_l, duty_l);
    ESP_LOGI("MOTOR","set cmpr_l -> %d (err=%s)", duty_l, esp_err_to_name(e));
    mcpwm_timer_start_stop(m_timer,MCPWM_TIMER_START_NO_STOP);

    vTaskDelay(pdMS_TO_TICKS(1000));
    printf("mot_on: timer_stop\n");
	mcpwm_timer_start_stop(m_timer, MCPWM_TIMER_STOP_EMPTY);
	// その後ジェネレータ pin を普通のGPIOとして操作
	gpio_set_level((gpio_num_t)GEN_GPIO_NUM_r, 0); // 停止
	gpio_set_level((gpio_num_t)GEN_GPIO_NUM_l, 0);
}

void init_moter(){
    gpio_set_level(GPIO_NUM_33, 0); //l
    gpio_set_level(GPIO_NUM_18, 1); //r
    setup_motor();
}


