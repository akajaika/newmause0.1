#include "parameters.hpp"
#include "glob_ver.hpp"
#include "static_parameters.hpp"
#include "typedef.hpp"
#include "motor.hpp"
#include "MA732.hpp"


void intr_cmt0() {
    if (run_mode == STRAIGHT_MODE) {
        // Handle straight mode interrupt
        // Add your specific logic here
        tar_speed += accel / 1000; 
        if (tar_speed > max_speed) {
            tar_speed = max_speed; 
        }
    } else if (run_mode == TURN_MODE) {
        // Handle turn mode interrupt
        // Add your specific logic here
        tar_speed += accel / 1000;
        if (tar_speed > max_speed) {
            tar_speed = max_speed; 
        }   
        
        tar_ang_vel += ang_acc / 1000;
        tar_degree += (tar_ang_vel * 180 / PI ) / 1000.0;

        if (TURN_DIR == LEFT) {
            if (tar_degree > max_degree) {
                tar_degree = max_degree;
            }
            if (tar_ang_vel > max_ang_vel) {
                tar_ang_vel = max_ang_vel;
            }
        } else if (TURN_DIR == RIGHT) {
            if (tar_degree < max_degree) {
                tar_degree = max_degree;
            }
            if (tar_ang_vel < -max_ang_vel) {
                tar_ang_vel = -max_ang_vel;
            }
        }
    }

    I_tar_speed += tar_speed;
	if(I_tar_speed >30*10000000000){
		I_tar_speed = 30*10000000000;
	}else if(I_tar_speed < -1*10000000000){
		I_tar_speed = 1*10000000000;
	} 
    I_tar_ang_vel += tar_ang_vel;
    if (I_tar_ang_vel > 30*10000000000) {
        I_tar_ang_vel = 30*10000000000;
    } else if (I_tar_ang_vel < -1*10000000000) {
        I_tar_ang_vel = 1*10000000000;
    }   

    V_r = V_l = 0; // Reset velocities for the next cycle
    
    if (run_mode == STRAIGHT_MODE || run_mode == TURN_MODE) {
        //PID speed
        V_r += (tar_speed - speed) * SPEED_KP;
        V_l += (tar_speed - speed) * SPEED_KP;

        V_r += (I_tar_speed - I_speed) * SPEED_KI;
        V_l += (I_tar_speed - I_speed) * SPEED_KI;

        V_r -= (p_speed - speed) * SPEED_KD;
        V_l -= (p_speed - speed) * SPEED_KD;

        //PID omega
        V_r += (tar_ang_vel - ang_vel) * OMEGA_KP;
        V_l -= (tar_ang_vel - ang_vel) * OMEGA_KP;

        V_r += (I_tar_ang_vel - I_ang_vel) * OMEGA_KI;
        V_l -= (I_tar_ang_vel - I_ang_vel) * OMEGA_KI;

        V_r -= (p_ang_vel - ang_vel) * OMEGA_KD;
        V_l += (p_ang_vel - ang_vel) * OMEGA_KD;
    
    }

    if(V_r > 3.3) {
        V_r = 3.3; // Limit to max voltage
    } 
    if(V_l > 3.3) {
        V_l = 3.3; // Limit to max voltage
    }

    Duty_r = V_r / V_bat;
    Duty_l = V_l / V_bat;

	motor.motor_r.duty =(short)(500.0 * Duty_r);
	motor.motor_l.duty =(short)(500.0 * Duty_l);

    mot_on(motor.motor_r.duty, motor.motor_l.duty);

    timer++;
	cnt++;
}

void int_cmt2(void)
{
    static unsigned int enc_data_r; // エンコーダ右のデータ
    static unsigned int enc_data_l; // エンコーダ左のデータ 
    static short state;

    /*****************************************************************************************
     エンコーダの読み取り：
        値を取得 → 角度更新 → 速度計算
    *****************************************************************************************/    
    if (state == 0) {
        MA732_read();// エンコーダ角度読み取り
        enc_data_r = angle_r;
        state = 1;
    } else {
        MA732_read();   // エンコーダ角度読み取り
        enc_data_l = angle_l;

        // 現在のエンコーダ角度（4096分解能、360度 = 0度とする）
        locate_r = enc_data_r;
        locate_l = enc_data_l;

        // 右エンコーダの差分パルス計算（1msあたりの移動量）
        diff_pulse_r = (locate_r - before_locate_r);
        // オーバーフロー処理（時計回り回転時）
        if ((diff_pulse_r > ENC_RES_HALF || diff_pulse_r < -ENC_RES_HALF) && before_locate_r > ENC_RES_HALF) {
            diff_pulse_r = ((ENC_RES_MAX - 1) - before_locate_r) + locate_r;
        }
        // オーバーフロー処理（反時計回り回転時）
        else if ((diff_pulse_r > ENC_RES_HALF || diff_pulse_r < -ENC_RES_HALF) && before_locate_r <= ENC_RES_HALF) {
            diff_pulse_r = before_locate_r + ((ENC_RES_MAX - 1) - locate_r);
        }

        // 左エンコーダの差分パルス計算（1msあたりの移動量）
        diff_pulse_l = (-locate_l + before_locate_l);
        // オーバーフロー処理（時計回り回転時）
        if ((diff_pulse_l > ENC_RES_HALF || diff_pulse_l < -ENC_RES_HALF) && before_locate_l > ENC_RES_HALF) {
            diff_pulse_l = ((ENC_RES_MAX - 1) - before_locate_l) + locate_l;
        }
        // オーバーフロー処理（反時計回り回転時）
        else if ((diff_pulse_l > ENC_RES_HALF || diff_pulse_l < -ENC_RES_HALF) && before_locate_l <= ENC_RES_HALF) {
            diff_pulse_l = before_locate_l + ((ENC_RES_MAX - 1) - locate_l);
        }

        // 実速度算出（エンコーダ値 × mm/pulse）
        speed_new_r = (float)(diff_pulse_r * MMPP);
        speed_new_l = (float)(diff_pulse_l * MMPP);

        // 過去の速度保存
        speed_old_r = speed_r;
        speed_old_l = speed_l;

        // 単純移動平均フィルタ
        speed_r = speed_new_r * 0.1 + speed_old_r * 0.9;
        speed_l = speed_new_l * 0.1 + speed_old_l * 0.9;

        p_speed = speed;
        // 全体速度（左右の平均）
        speed = (speed_r + speed_l) / 2.0;

        // 積分項（飽和あり）
        I_speed += speed;
        if (I_speed > 30 * 10000000000) {
            I_speed = 30 * 10000000000;
        } else if (I_speed < -1 * 10000000000) {
            I_speed = -1 * 10000000000;
        }

        // 総移動距離（左右の新速度の平均を足す）
        len_mouse += (speed_new_r + speed_new_l) / 2.0;

        // 次回差分計算用に保持
        before_locate_r = locate_r;
        before_locate_l = locate_l;

        state = 0;
    }

    /*****************************************************************************************
     ジャイロ読み取り（角速度）：
        値取得 → ラジアン変換 → 積分 → 角度更新
    *****************************************************************************************/
    if (state == 1) {
        // ジャイロ値更新
        preprocess_spi_gyro(0xB70000);

        // ローパスフィルタ適用前の値取得
        gyro_x_new = (float)((short)(read_gyro_data() & 0x0000FFFF));
        gyro_x = (gyro_x_new - gyro_ref);

        // 角速度の更新
        p_ang_vel = ang_vel;
        ang_vel = ((2000.0 * gyro_x) / 32767.0) * PI / 180.0;

        // 積分項（飽和付き）
        I_ang_vel += ang_vel;
        if (I_ang_vel > 30 * 10000000000) {
            I_ang_vel = 30 * 10000000000;
        } else if (I_ang_vel < -1 * 10000000000) {
            I_ang_vel = -1 * 10000000000;
        }

        // 角度の更新（度単位）
        degree += (2.0 * (gyro_x_new - gyro_ref) / 32767.0);
    }    
}