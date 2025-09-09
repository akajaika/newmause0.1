#include "typedef.h"
#include "parameters.h"
#include "driver/mcpwm_cmpr.h"

#ifdef _GLOB_VAR
#define GLOBAL
#else
#define GLOBAL extern
#endif

// -----------------------------------------------------------------------------
// 構造体型のグローバル変数（センサ・制御・位置・地図など）
// -----------------------------------------------------------------------------
GLOBAL t_sensor         sen_r, sen_l, sen_fr, sen_fl;   // センサ（右、左、前右、前左）
GLOBAL t_control        con_wall;                       // 壁制御用構造体
GLOBAL t_control        con_fwall;                      // 前壁制御用構造体
GLOBAL t_position       mypos;                          // 自車位置
GLOBAL t_wall           wall[MAZESIZE_X][MAZESIZE_Y];   // 迷路の壁情報
GLOBAL unsigned char    map[MAZESIZE_X][MAZESIZE_Y];    // マップデータ

// -----------------------------------------------------------------------------
// 実行制御用
// -----------------------------------------------------------------------------
GLOBAL int run_mode;	                                // 走行モード

// -----------------------------------------------------------------------------
// 動作パラメータ（速度・角速度・角度など）
// -----------------------------------------------------------------------------
GLOBAL float fast_speed;      // 高速走行時の目標速度 [m/s]
GLOBAL float tar_ang_vel;     // 目標角速度 [rad/s]
GLOBAL float tar_degree;      // 目標角度 [deg]
GLOBAL float max_degree;      // 最大角度 [deg]
GLOBAL float start_degree;    // 開始角度 [deg]
GLOBAL float global_degree;   // 全体角度（用途に応じて）
GLOBAL float ang_vel;         // 現在の角速度 [rad/s]
GLOBAL float p_ang_vel;       // 前回の角速度 [rad/s]
GLOBAL float max_ang_vel;     // 最大角速度 [rad/s]
GLOBAL float ang_acc;         // 角加速度 [rad/s^2]
GLOBAL float accel;           // 直線加速度 [m/s^2]
GLOBAL float max_speed;       // 最大速度 [m/s]
GLOBAL float speed_r;         // 右車輪速度 [m/s]
GLOBAL float speed_l;         // 左車輪速度 [m/s]
GLOBAL float speed_old_r;     // 右車輪前回速度 [m/s]
GLOBAL float speed_new_r;     // 右車輪瞬時速度 [m/s]
GLOBAL float speed_old_l;     // 左車輪前回速度 [m/s]
GLOBAL float speed_new_l;     // 左車輪瞬時速度 [m/s]
GLOBAL float speed;           // 車体前進速度（平均） [m/s]
GLOBAL float p_speed;         // 前回の車体速度 [m/s]
GLOBAL float tar_speed;       // 目標速度 [m/s]
GLOBAL float end_speed;       // 終了速度 [m/s]
GLOBAL float V_r;             // 右モータ指令電圧 [V]
GLOBAL float V_l;             // 左モータ指令電圧 [V]

// -----------------------------------------------------------------------------
// エンコーダ関連
// -----------------------------------------------------------------------------
GLOBAL unsigned int angle_r, angle_l;    // （必要に応じて角度格納）
GLOBAL unsigned int locate_l;            // 左エンコーダ生値 [counts]
GLOBAL unsigned int locate_r;            // 右エンコーダ生値 [counts]
GLOBAL unsigned int before_locate_r;     // 右エンコーダ前回値 [counts]
GLOBAL unsigned int before_locate_l;     // 左エンコーダ前回値 [counts]
GLOBAL int diff_pulse_r;                 // 右差分パルス（pulse / ms）
GLOBAL int diff_pulse_l;                 // 左差分パルス（pulse / ms）

// -----------------------------------------------------------------------------
// 各種フラグ・制御補助変数
// -----------------------------------------------------------------------------
GLOBAL int turn_con;
GLOBAL int con_con;
GLOBAL int wallcut;
GLOBAL int wallcut_con;
GLOBAL int wallcut_con_before;
GLOBAL int wallcut_do;
GLOBAL int sen_l_before;   // センサ履歴（左）
GLOBAL int sen_r_before;   // センサ履歴（右）

// -----------------------------------------------------------------------------
// タイマー用
// -----------------------------------------------------------------------------
GLOBAL volatile unsigned int timer;  // 1 ms タイマーカウンタ

// -----------------------------------------------------------------------------
// 電源・電圧など
// -----------------------------------------------------------------------------
GLOBAL long cnt;      // 汎用カウンタ
GLOBAL float V_bat;   // バッテリ電圧 [V]

// -----------------------------------------------------------------------------
// ジャイロ関連
// -----------------------------------------------------------------------------
GLOBAL float gyro_x;       // ジャイロ X 生データ（補正済） [counts]
GLOBAL float gyro_x_new;   // ジャイロ X 新規生データ [counts]
GLOBAL float gyro_ref;     // ジャイロ基準オフセット [counts]
GLOBAL float degree;       // 積分した角度 [degree]

// -----------------------------------------------------------------------------
// 距離関連
// -----------------------------------------------------------------------------
GLOBAL float len_mouse;    // 走行距離 [mm]
GLOBAL float len_target;   // 目標距離 [mm]

// -----------------------------------------------------------------------------
// ログ出力
// -----------------------------------------------------------------------------
GLOBAL int logger[12][LOG_CNT];  // ログ配列
GLOBAL long log_timer;           // ログタイマ
GLOBAL int log_flag;             // ログ取得フラグ
GLOBAL int debug_flag;

// -----------------------------------------------------------------------------
// フラグなど
// -----------------------------------------------------------------------------
GLOBAL char TURN_DIR;  // 回転方向フラグ

// -----------------------------------------------------------------------------
// モーター出力デューティ / MCPWM 比較ハンドル
// -----------------------------------------------------------------------------
GLOBAL float Duty_r;   // 右PWM デューティ [%]
GLOBAL float Duty_l;   // 左PWM デューティ [%]
GLOBAL mcpwm_cmpr_handle_t cmpr_r;
GLOBAL mcpwm_cmpr_handle_t cmpr_l;

// -----------------------------------------------------------------------------
// 積分項（PID関連）
// -----------------------------------------------------------------------------
GLOBAL float I_tar_speed;   // 目標速度の I 和
GLOBAL float I_speed;       // 速度の I 和
GLOBAL float I_tar_ang_vel; // 目標角速度の I 和
GLOBAL float I_ang_vel;     // 角速度の I 和

// -----------------------------------------------------------------------------
// UI / その他
// -----------------------------------------------------------------------------
GLOBAL unsigned char push_switch; // 押しボタン状態

GLOBAL int next_nextdir;
GLOBAL next_position next;
GLOBAL bool sidewallcheck_flag;
