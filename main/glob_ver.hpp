#include "typedef.hpp"
#include "parameters.hpp"

#ifdef _GLOB_VAR
#define GLOBAL
#else
#define GLOBAL extern
#endif

// センサ・制御・位置・地図など
GLOBAL t_sensor         sen_r, sen_l, sen_fr, sen_fl;
GLOBAL t_control        con_wall;
GLOBAL t_control        con_fwall;
GLOBAL t_position       mypos;
GLOBAL t_wall           wall[MAZESIZE_X][MAZESIZE_Y];
GLOBAL unsigned char    map[MAZESIZE_X][MAZESIZE_Y];

// 実行制御用
GLOBAL int run_mode;

// 動作パラメータ
GLOBAL float fast_speed;
GLOBAL float tar_ang_vel;
GLOBAL float tar_degree;
GLOBAL float max_degree;
GLOBAL float start_degree;
GLOBAL float global_degree;
GLOBAL float ang_vel;
GLOBAL float p_ang_vel;
GLOBAL float max_ang_vel;
GLOBAL float ang_acc;
GLOBAL float accel;
GLOBAL float max_speed;
GLOBAL float speed_r;
GLOBAL float speed_l;
GLOBAL float speed_old_r;
GLOBAL float speed_new_r;
GLOBAL float speed_old_l;
GLOBAL float speed_new_l;
GLOBAL float speed;
GLOBAL float p_speed;
GLOBAL float tar_speed;
GLOBAL float end_speed;
GLOBAL float V_r;
GLOBAL float V_l;

// エンコーダ関連
GLOBAL unsigned int angle_r, angle_l;
GLOBAL unsigned int locate_l;
GLOBAL unsigned int locate_r;
GLOBAL unsigned int before_locate_r;
GLOBAL unsigned int before_locate_l;
GLOBAL int diff_pulse_r;
GLOBAL int diff_pulse_l;

GLOBAL int turn_con;
GLOBAL int con_con;
GLOBAL int wallcut;
GLOBAL int wallcut_con;
GLOBAL int wallcut_con_before;
GLOBAL int wallcut_do;
GLOBAL int sen_l_before;
GLOBAL int sen_r_before;

// タイマー用
GLOBAL volatile unsigned int timer;

// 電源・電圧など
GLOBAL long cnt;
GLOBAL float V_bat;

// ジャイロ
GLOBAL float gyro_x;
GLOBAL float gyro_x_new;
GLOBAL float gyro_ref;
GLOBAL float degree;

// 距離
GLOBAL float len_mouse;
GLOBAL float len_target;

// ログ出力
GLOBAL int log[12][LOG_CNT];
GLOBAL long log_timer;
GLOBAL int log_flag;
GLOBAL int debug_flag;

// フラグなど
GLOBAL char TURN_DIR;

// モーター出力デューティ
GLOBAL float Duty_r;
GLOBAL float Duty_l;

// 積分項など
GLOBAL float I_tar_speed;
GLOBAL float I_speed;
GLOBAL float I_tar_ang_vel;
GLOBAL float I_ang_vel;

// UI
GLOBAL unsigned char push_switch;

GLOBAL int next_nextdir;
GLOBAL next_position next;
GLOBAL bool sidewallcheck_flag;