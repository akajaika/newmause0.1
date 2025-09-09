#ifndef MOTER_TEST_H
#define MOTER_TEST_H

#include "driver/mcpwm_timer.h"

extern void mot_on(short duty_r, short duty_l);
extern void init_moter(void);

extern mcpwm_timer_handle_t m_timer;
extern mcpwm_oper_handle_t oper;
extern mcpwm_gen_handle_t gen_r,gen_l;

#endif