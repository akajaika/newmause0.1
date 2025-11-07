#define _GLOB_VAR
#include "glob_ver.h"

#include "driver/mcpwm_cmpr.h"

mcpwm_cmpr_handle_t cmpr_r = NULL;
mcpwm_cmpr_handle_t cmpr_l = NULL;

float dt = 0.0f;