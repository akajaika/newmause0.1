#include "driver/mcpwm_timer.h"
#include "driver/mcpwm_oper.h"
#include "driver/mcpwm_gen.h"
#include "driver/mcpwm_cmpr.h"
#include "driver/gpio.h"


#define MCPWM_GEN_COMPARE_EVENT_ACTION_COUNT 1
#define GEN_GPIO_NUM_r 38
#define GEN_GPIO_NUM_l 17

#define NUM_TIMER_ACTIONS 1

#define duty_r_ticks 250 // デューティサイクルの初期値 (0-500)
#define duty_l_ticks 250 // デューティサイクルの初期値 (0-500)
#define new_duty_r 100 // 新しいデューティサイクル (0-500)
#define new_duty_l 100 // 新しいデューティサイクル (0-500)

void setup_motor(){
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
        .period_ticks = 500, 
        .intr_priority = 0, 
        .flags = {
            .update_period_on_empty = false,
            .update_period_on_sync = false,
        },
    };
    mcpwm_timer_handle_t timer;


    //コンパレーター初期化
    mcpwm_comparator_config_t mcpwm_new_comparator_cfg = {
        .intr_priority = 0,
    };
    mcpwm_cmpr_handle_t cmpr_r,cmpr_l;


    //オペレーター初期化
    mcpwm_operator_config_t mcpwm_new_operator_cfg = {
        .group_id = 0,
        .intr_priority = 0,
    };
    mcpwm_oper_handle_t oper;
    

    //ジェネレーター初期化
    mcpwm_generator_config_t mcpwm_new_generator_cfg_r = {
        .gen_gpio_num = GEN_GPIO_NUM_r
    };
    mcpwm_generator_config_t mcpwm_new_generator_cfg_l = {
        .gen_gpio_num = GEN_GPIO_NUM_l
    };
    mcpwm_gen_handle_t gen_r,gen_l;
    mcpwm_gen_timer_event_action_t gen_timer_action = {
        .direction = MCPWM_TIMER_DIRECTION_UP,
        .event = MCPWM_TIMER_EVENT_EMPTY,
        .action = MCPWM_GEN_ACTION_HIGH,
    };
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

    
    mcpwm_new_timer(&mcpwm_new_timer_cfg,&timer); 
    mcpwm_timer_enable(timer);
    mcpwm_timer_start_stop(timer,MCPWM_TIMER_START_NO_STOP);

    mcpwm_new_operator(&mcpwm_new_operator_cfg,&oper);

    //timer,oper接続
    mcpwm_operator_connect_timer(oper,timer);

    mcpwm_new_comparator(oper,&mcpwm_new_comparator_cfg,&cmpr_r);
    mcpwm_comparator_set_compare_value(cmpr_r,duty_r_ticks);
    mcpwm_new_comparator(oper,&mcpwm_new_comparator_cfg,&cmpr_l);
    mcpwm_comparator_set_compare_value(cmpr_l,duty_l_ticks);

    mcpwm_new_generator(oper,&mcpwm_new_generator_cfg_r,&gen_r);
    mcpwm_new_generator(oper,&mcpwm_new_generator_cfg_l,&gen_l);

    mcpwm_generator_set_actions_on_timer_event(gen_r,gen_timer_action,NUM_TIMER_ACTIONS);
    mcpwm_generator_set_actions_on_compare_event(gen_r,gen_cmpr_action_r,MCPWM_GEN_COMPARE_EVENT_ACTION_COUNT);
    mcpwm_generator_set_actions_on_timer_event(gen_l,gen_timer_action,NUM_TIMER_ACTIONS);
    mcpwm_generator_set_actions_on_compare_event(gen_l,gen_cmpr_action_l,MCPWM_GEN_COMPARE_EVENT_ACTION_COUNT);

    mcpwm_comparator_set_compare_value(cmpr_r, new_duty_r);
    mcpwm_comparator_set_compare_value(cmpr_l, new_duty_l);
}

void app_main(){
    gpio_set_level(GPIO_NUM_33, 0);
    gpio_set_level(GPIO_NUM_18, 1);
    setup_motor();
}
