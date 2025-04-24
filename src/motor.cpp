#include "driver/mcpwm_timer.h"
#include "driver/mcpwm_oper.h"
#include "driver/mcpwm_gen.h"
#include "driver/mcpwm_cmpr.h"


#define MCPWM_GEN_COMPARE_EVENT_ACTION_COUNT 1
#define GEN_GPIO_NUM 38

#define NUM_TIMER_ACTIONS 1

void setup_motor(){
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
    mcpwm_cmpr_handle_t cmpr;


    //オペレーター初期化
    mcpwm_operator_config_t mcpwm_new_operator_cfg = {
        .group_id = 0,
        .intr_priority = 0,
    };
    mcpwm_oper_handle_t oper;
    

    //ジェネレーター初期化
    mcpwm_generator_config_t mcpwm_new_generator_cfg = {
        .gen_gpio_num = GEN_GPIO_NUM
    };
    mcpwm_gen_handle_t gen;
    mcpwm_gen_timer_event_action_t gen_timer_action = {
        .direction = MCPWM_TIMER_DIRECTION_UP,
        .event = MCPWM_TIMER_EVENT_EMPTY,
        .action = MCPWM_GEN_ACTION_HIGH,
    };
    mcpwm_gen_compare_event_action_t gen_cmpr_action = {
        .direction = MCPWM_TIMER_DIRECTION_UP,
        .comparator = cmpr,
        .action = MCPWM_GEN_ACTION_LOW,
    };

    
    mcpwm_new_timer(&mcpwm_new_timer_cfg,&timer); 
    mcpwm_timer_enable(timer);
    mcpwm_timer_start_stop(timer,MCPWM_TIMER_START_NO_STOP);

    mcpwm_new_operator(&mcpwm_new_operator_cfg,&oper);

    //timer,oper接続
    mcpwm_operator_connect_timer(oper,timer);

    mcpwm_new_comparator(oper,&mcpwm_new_comparator_cfg,&cmpr);
    mcpwm_comparator_set_compare_value(cmpr,250);

    mcpwm_new_generator(oper,&mcpwm_new_generator_cfg,&gen);
    mcpwm_generator_set_actions_on_timer_event(gen,gen_timer_action,NUM_TIMER_ACTIONS);
    mcpwm_generator_set_actions_on_compare_event(gen,gen_cmpr_action,MCPWM_GEN_COMPARE_EVENT_ACTION_COUNT);
}

void app_main(){
    setup_motor();
}
