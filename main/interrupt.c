#include <math.h>
#include "parameters.h"
#include "glob_ver.h"
#include "static_parameters.h"
#include "typedef.h"
#include "moter_test.h"
#include "MA732_read.h"
#include "driver/mcpwm_types.h"
#include "driver/mcpwm_timer.h"
#include "driver/mcpwm_oper.h"
#include "driver/mcpwm_gen.h"
#include "driver/mcpwm_cmpr.h"
#include "driver/gpio.h"
#include "adc.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_err.h"
#include "test_ledc.h"
#include "esp_timer.h"


static const char *TAG = "ADC";

static int buffer_index = 0;
static int sen_r_history[300];
static int sen_l_history[300];
static int on_value = 0;
uint16_t values[8] = {0};

t_imu imu_ag;

int degree_r = 0;
int degree_l = 0;

int as = 1;
float duty_r,duty_l = 1.0;

int count_LOGE = 0;

static int64_t last_time = 0;

int test_duty_count = 700;

void int_cmt0() {
    if (run_mode == STRAIGHT_MODE) {
        // Handle straight mode interrupt
        // Add your specific logic here 
        tar_speed += accel / 1000;
        if (tar_speed > max_speed) {
            // ESP_LOGI(TAG, "tar_speed: %f, max_speed: %f", tar_speed, max_speed);
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
            if (tar_ang_vel < max_ang_vel) {
                tar_ang_vel = max_ang_vel;
            }
        }
    }

    // if(run_mode == STRAIGHT_MODE){
	// 	if(con_wall.enable == true && sen_fr.value + sen_fl.value <= (TH_SEN_FR+TH_SEN_FL)*5 )		//?????????????????`?F?b?N
	// 	{
			
	// 		con_wall.p_error = con_wall.error;	//???????????
			
			
	// 		//???E??Z???T???A??????g????????????????`?F?b?N????A?p????????????v?Z
	// 		if( ( sen_r.is_control == true ) && ( sen_l.is_control == true ) )
	// 		{									//????????L?????????????????v?Z
	// 			con_wall.error = sen_r.error - sen_l.error;
	// 		}
	// 		else								//?Е??????????????Z???T?????????????????????v?Z
	// 		{
	// 			con_wall.error = 2.0 * (sen_r.error - sen_l.error);	//?Е??????g?p????????2?{????
	// 		}
			
			
	// 		//DI????v?Z
	// 		con_wall.diff = con_wall.error - con_wall.p_error;	//?????????l???v?Z
	// 		con_wall.sum += con_wall.error;				//????????l???v?Z
			
	// 		if(con_wall.sum > con_wall.sum_max)			//????????l????l???
	// 		{
	// 			con_wall.sum = con_wall.sum_max;
	// 		}
	// 		else if(con_wall.sum < (-con_wall.sum_max))		//????????l????l???
	// 		{
	// 			con_wall.sum = -con_wall.sum_max;
	// 		}

	// 		con_wall.p_omega = con_wall.omega;
	// 		con_wall.omega = con_wall.kp * con_wall.error * 0.5 + con_wall.p_omega * 0.5;	//??????W?p???x[rad/s]???v?Z
	// 		tar_ang_vel = con_wall.omega;
	// 	}else{
	// 		tar_ang_vel = 0;
	// 	}
		
	// }


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
        // V_r += (tar_speed - speed) * SPEED_KP;
        // V_l += (tar_speed - speed) * SPEED_KP;
        
        V_r += (tar_speed) * SPEED_KP;
        V_l += (tar_speed) * SPEED_KP;

        // printf("Doing1 intrrept r %f\n", V_r);
        // printf("Doing1 intrrept l %f\n", V_l);

        // V_r += (I_tar_speed - I_speed) * SPEED_KI;
        // V_l += (I_tar_speed - I_speed) * SPEED_KI;

        // printf("Doing2 intrrept r %f\n", V_r);
        // printf("Doing2 intrrept l %f\n", V_l);

        // V_r -= (p_speed - speed) * SPEED_KD;
        // V_l -= (p_speed - speed) * SPEED_KD;
        
        // Straight Balance control
        if (run_mode == STRAIGHT_MODE)
        {
            V_r -= (speed_r - speed_l) * BALANCE_GAIN_P;
            V_l -= (speed_l - speed_r) * BALANCE_GAIN_P;

            // V_r += (p_speed_r - speed_r) * BALANCE_GAIN_D;
            // V_l += (p_speed_l - speed_l) * BALANCE_GAIN_D;
        }

        // printf("Doing3 intrrept r %f\n", V_r);
        // printf("Doing3 intrrept l %f\n", V_l);
        //PID omega
        // V_r += (tar_ang_vel - ang_vel) * OMEGA_KP;
        // V_l -= (tar_ang_vel - ang_vel) * OMEGA_KP;

        // V_r += (tar_ang_vel) * OMEGA_KP;
        // V_l -= (tar_ang_vel) * OMEGA_KP;

        // V_r += (I_tar_ang_vel - I_ang_vel) * OMEGA_KI;
        // V_l -= (I_tar_ang_vel - I_ang_vel) * OMEGA_KI;

        // V_r -= (p_ang_vel - ang_vel) * OMEGA_KD;
        // V_l += (p_ang_vel - ang_vel) * OMEGA_KD;

        // if (run_mode == TURN_MODE)
        // {
        //     V_r -= (speed_r - (-speed_l)) * BALANCE_GAIN_P;
        //     V_l -= (speed_l - (-speed_r)) * BALANCE_GAIN_P;

        //     // V_r += (p_speed_r - (-speed_r)) * BALANCE_GAIN_D;
        //     // V_l += (p_speed_l - (-speed_l)) * BALANCE_GAIN_D;
        // }
        
    }

    if(V_r > 2.0) {
        V_r = 2.0; // Limit to max voltage
        // printf("Limit to max voltage\n");
    } 
    if(V_l > 2.0) {
        V_l = 2.0; // Limit to max voltage
    }

    // if(V_l < 0) gpio_set_level(GPIO_NUM_33, 1);
    // else gpio_set_level(GPIO_NUM_33, 0);    
    // if(V_r < 0) gpio_set_level(GPIO_NUM_18, 0);
    // else gpio_set_level(GPIO_NUM_18, 1);


    // printf("Doing intrrept V_bat %f\n", V_bat);
    // if(run_mode == TURN_MODE){
    //     // if(TURN_DIR == LEFT){
    //     //     if(V_l < 0) gpio_set_level(GPIO_NUM_33, 1);
    //     //     else V_l = 0;
    //     //     if(V_r < 0) V_r = 0; 
    //     //     else gpio_set_level(GPIO_NUM_18, 1);
    //     // }else if(TURN_DIR == RIGHT){    
    //     //     if(V_l < 0) V_l = 0; 
    //     //     else gpio_set_level(GPIO_NUM_33, 0);    
    //     //     if(V_r < 0) gpio_set_level(GPIO_NUM_18, 0);
    //     //     else V_r = 0;
    //     // }
    // }else{
    //     if(V_l < 0){ V_l = 0;}; 
    //     if(V_r < 0){ V_r = 0;};
    //     // if(V_l < 0){ V_l = 0;printf("latest");}; 
    //     // if(V_r < 0){ V_r = 0;printf("latest");};
    // }

    // Duty_r = (fabs(V_r)+0.25) / 4.2;
    // Duty_l = (fabs(V_l)+0.25) / 4.2;
    Duty_r = (V_r) / (V_bat*2);
    Duty_l = (V_l) / (V_bat*2);

    // Duty_r = (fabs(V_r)+0.25) / V_bat;
    // Duty_l = (fabs(V_l)+0.25) / V_bat;
    // Duty_l = (fabs(V_l))*1.2 / 4.2;

    // Duty_r = 0.19999999/2.1;
    // Duty_l = 0.19999999/2.1;

    // printf("Doing intrrept Duty_r %f\n", Duty_r);

	motor.motor_r.duty =((short)(953.0 * Duty_r) + 60);
	motor.motor_l.duty =((short)(953.0 * Duty_l) + 60);

    // motor.motor_r.duty =150;
	// motor.motor_l.duty =800;

    
    // if (test_duty_count < 1000.0){
    //     if (motor.status == true){
    //         test_duty_count++;
    //         // update_duty(test_duty_count/10, test_duty_count/10);
    //         update_duty(80, 80);
    //     }
    // }else{
    //     update_duty(0, 0);
    //     test_duty_count = 1000001.0;
    // }


    if(motor.status == true) {
        // printf("If motors are enabled, apply the duty cycle\n\n");
        // mcpwm_timer_start_stop(m_timer,MCPWM_TIMER_START_NO_STOP);
        // printf("motors are enabled\n\n");
        update_duty(motor.motor_r.duty, motor.motor_l.duty);
        // printf("motors are doing\n\n");
    } else {
        // printf(" motors are disabled, set duty to 0\n\n");
        motor.motor_r.duty = 0;
        motor.motor_l.duty = 0;
        update_duty(motor.motor_r.duty, motor.motor_l.duty);
        // if (m_timer == NULL) {
        //     return;
        // }else {
        //     mcpwm_timer_start_stop(m_timer,MCPWM_TIMER_STOP_EMPTY);
        // }
    }

    timer++;
	cnt++;

    int64_t now = esp_timer_get_time(); 
    dt = (now - last_time) / 1000000.0f;
    last_time = now;
}

void int_cmt1(void)
{

	gpio_set_level(GPIO_NUM_5, 0);
	gpio_set_level(GPIO_NUM_7, 0);
	gpio_set_level(GPIO_NUM_9, 0); //LED付いてない
	gpio_set_level(GPIO_NUM_14, 0);


	// 	switch (as)
	// 	{
	// 	case 1:
	// 		printf("Doing case 1...\n\n");
	// 		gpio_set_level(GPIO_NUM_14, 1); // LED消灯
    //         gpio_set_level(GPIO_NUM_5, 0);
	// 		// ledc_test_pwm2(GPIO_NUM_5, 1); // LED点灯
	// 		as = 2; // Increment to next case
	// 	    sen_r.d_value = adc_read(1);
	// 		break;

	// 	case 2:
	// 		printf("Doing case 2...\n\n");
	// 		// gpio_set_level(GPIO_NUM_5, 1); // LED消灯
    //         gpio_set_level(GPIO_NUM_5, 1);
	// 		gpio_set_level(GPIO_NUM_7, 0); // LED点灯	
    //         // ledc_test_pwm2(GPIO_NUM_7, 1); 
	// 		as = 3; // Increment to next case
    //         sen_l.d_value = adc_read(2);
	// 		break;
	// 	case 3:
	// 		printf("Doing case 3...\n\n");
	// 		gpio_set_level(GPIO_NUM_7, 1); // LED消灯
    //         gpio_set_level(GPIO_NUM_9, 0);
	// 		// ledc_test_pwm2(GPIO_NUM_9, 1);  // LED点灯
	// 		as = 4; // Increment to next case
    //         sen_fr.d_value = adc_read(3);
	// 		break;	

	// 	case 4:
	// 		printf("Doing case 4...\n\n");
	// 		gpio_set_level(GPIO_NUM_9, 1); // LED消灯
    //         // ESP_LOGI(TAG, "CHnum:%4d \n",values[1]);
    //         gpio_set_level(GPIO_NUM_14, 0);
	// 		// ledc_test_pwm2(GPIO_NUM_14, 1); // LED点灯
	// 		as = 1; // Reset to 0 to cycle through cases
    //         sen_fr.d_value = adc_read(4);
	// 		break;
		
	// 	default:
	// 		break;
            
	//     }

    // printf("CH0:%4d CH1:%4d CH2:%4d CH3:%4d\n"
    //        "CH4:%4d CH5:%4d CH6:%4d CH7:%4d\n",
    //             values[0], values[1], values[2], values[3] ,
    //             values[4], values[5], values[6], values[7]);

    static int state = 0;
    int i;

    switch(state)
	{
		case 0:		//右センサの読み取り

            // バッファに現在のセンサ値を保存
			sen_r_history[buffer_index] = sen_r.value;

			// 指定した周期前の値を sen_r_before に保存
			int past_r_index = (buffer_index + 300 - 120) % 300; // 3周期前の値
			sen_r_before = sen_r_history[past_r_index];
			
			buffer_index = (buffer_index + 1) % 300;

			//バッファ設
			sen_r.d_value = adc_read(1);			//暗電圧取得
            values[0] = sen_r.d_value; // Store the value for logging
            
            // printf("sen_r.d_value = %d\n", sen_r.d_value);
			
			gpio_set_level(GPIO_NUM_5, 0);				//LED点灯
			for(i = 0; i < WAITLOOP_SLED*10; i++);		//フォトトランジスタ安定化のためのウェイト
            on_value = adc_read(1); // Read the ADC value for the right sensor
            values[4] = on_value; // Store the value for logging
			gpio_set_level(GPIO_NUM_5, 1);				//LED消灯
	
			sen_r.value = (on_value - sen_r.d_value);	//センサ値取得

            // ESP_LOGE(TAG, "ADC Read: sen_r, Value: %d", sen_r.value);

			if(sen_r.value > sen_r.th_wall)			//壁判定
			{
				sen_r.is_wall = true;			//右壁あり
			}
			else
			{
				sen_r.is_wall = false;			//右壁なし
			}
			
			if(sen_r.value > sen_r.th_control)		//制御用しきい値を超えたら
			{
				sen_r.error = sen_r.value - sen_r.ref;	//偏差計算
				sen_r.is_control = true;		//制御対象にする
			}
			else
			{
				sen_r.error = 0;			//制御対象外
				sen_r.is_control = false;
			}		
            state = 3;		//次の状態へ移行	
			break;


		case 1:		//左前センサの読み取り

			//バッファ設定
			sen_fl.d_value = adc_read(3);			//暗電圧取得
            values[3] = sen_fl.d_value; // Store the value for logging
		
			gpio_set_level(GPIO_NUM_9, 0);					//LED点灯
			for(i = 0; i < WAITLOOP_SLED*10; i++);		//フォトトランジスタ安定化ウェイト
            on_value = adc_read(3);
            values[7] = on_value; // Store the value for logging
			gpio_set_level(GPIO_NUM_9, 1);					//LED消灯

			sen_fl.value = (on_value - sen_fl.d_value);	//センサ値取得

			if(sen_fl.value > sen_fl.th_wall)		//壁判定
			{
				sen_fl.is_wall = true;			//左前壁あり
			}
			else
			{
				sen_fl.is_wall = false;			//左前壁なし
			}
            state = 0;
			break;


		case 2:		//右前センサの読み取り
		
			//バッファ設定
			sen_fr.d_value = adc_read(0);			//暗電圧取得
            values[2] = sen_fr.d_value; // Store the value for logging
		
			gpio_set_level(GPIO_NUM_14, 0);					//LED点灯
			for(i = 0; i < WAITLOOP_SLED*10; i++);		//フォトトランジスタ安定化
            on_value = adc_read(0);
            values[6] = on_value; // Store the value for logging
			gpio_set_level(GPIO_NUM_14, 1);					//LED消灯
			
			sen_fr.value = (on_value - sen_fr.d_value);	//センサ値取得

			if(sen_fr.value > sen_fr.th_wall)		//壁判定
			{
				sen_fr.is_wall = true;			//右前壁あり
			}
			else
			{
				sen_fr.is_wall = false;			//右前壁なし
			}	
            state = 1;		
			break;


		case 3:		//左センサの読み取り

			sen_l_history[buffer_index] = sen_l.value;
			
			// 指定した周期前の値を sen_r_before に保存
			int past_l_index = (buffer_index + 300 - 120) % 300; // 3周期前の値
			sen_l_before = sen_l_history[past_l_index];

			buffer_index = (buffer_index + 1) % 300;
			
			//バッファ設定
			sen_l.d_value = adc_read(2);			//暗電圧取得
            values[1] = sen_l.d_value; // Store the value for logging
			
			gpio_set_level(GPIO_NUM_7, 0);					//LED点灯
			for(i = 0; i < WAITLOOP_SLED*10; i++)	;	//フォトトランジスタ安定化
            on_value = adc_read(2);
            values[5] = on_value; // Store the value for logging
			gpio_set_level(GPIO_NUM_7, 1);					//LED消灯
			
			sen_l.value = (on_value - sen_l.d_value);	//センサ値取得
			
			if(sen_l.value > sen_l.th_wall)			//壁判定
			{
				sen_l.is_wall = true;			//左壁あり
			}
			else
			{
				sen_l.is_wall = false;			//左壁なし
			}
			
			if(sen_l.value > sen_l.th_control)		//制御対象か
			{
				sen_l.error = sen_l.value - sen_l.ref;	//偏差計算
				sen_l.is_control = true;		//制御対象にする
			}
			else
			{
				sen_l.error = 0;			//制御対象外
				sen_l.is_control = false;
			}
            state = 2;
			break;
	}

            // if(count_LOGE == 10){
        //     // ESP_LOGI(TAG_MPU, "Accel: X=%d Y=%d Z=%d", ax, ay, az);
        //     ESP_LOGE(TAG, "gyro_x_new: %f, dgree: %f", gyro_x_new, degree);
        //     ESP_LOGE(TAG, "gyro_x_new: %f, dgree: %f", gyro_x_new, degree);
        //     ESP_LOGE(TAG, "len_mouse: %f", len_mouse);
        //     printf("\x1b[2J");
        //     printf("\x1b[0;0H");
        //     count_LOGE = 0;
        // }else{
        //     count_LOGE++;
        // }

    // V_bat = ((float)adc_read(4) * ((2400.0 + 620.0) / 620.0)); // Read battery voltage
    V_bat = ((float)adc_read(4)) / 4095.0 * 1.1 * (2400.0 + 620.0) / 620.0; // Read battery voltage
    V_bat = V_bat-0.06; // Convert to volts
    V_bat = V_bat * ((V_bat/4.2)*(V_bat/4.2)); // キャリブレーション
    V_bat = V_bat +0.1; // キャリブレーション
    // ESP_LOGE(TAG, "V_bat: %f", V_bat);
    // printf("\x1b[2J");
    // printf("\x1b[0;0H");
	
	state++;		//4つの状態で切り替え
	if(state > 3)
	{
		state = 0;
	}


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
        // MA732_read();// エンコーダ角度読み取り

        enc_data_r = angle2;
        enc_data_l = angle1;

        // printf("MA732: Right=%.2d Left=%.2d\n", enc_data_r, enc_data_l);

        // 現在のエンコーダ角度（16383分解能14bit、360度 = 0度とする）
        locate_r = enc_data_r;
        locate_l = enc_data_l;

        degree_r = ma732_raw14_to_degree(angle1); // 右エンコーダの角度
        degree_l = ma732_raw14_to_degree(angle2); // 左エンコーダの角度

        // ESP_LOGE(TAG, "locate_r: %u, locate_l: %u", locate_r, locate_l);

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

        // ESP_LOGE(TAG, "diff_pulse_r: %d, diff_pulse_l: %d", diff_pulse_r, diff_pulse_l);
        // ESP_LOGE(TAG, "speed_new_r: %f, speed_new_l: %f", speed_new_r, speed_new_l);
        // printf("%4u,%4u,%6u,%6u,%10.6f,%10.6f\n", degree_r, degree_l, locate_r, locate_l, speed_new_r, speed_new_l);

        // 過去の速度保存
        speed_old_r = speed_r;
        speed_old_l = speed_l;

        // 単純移動平均フィルタ
        speed_r = speed_new_r * 0.1 + speed_old_r * 0.9;
        speed_l = speed_new_l * 0.1 + speed_old_l * 0.9;

        p_speed = speed;
        p_speed_r = speed_r;
        p_speed_l = speed_l;

        // 全体速度（左右の平均）
        speed = (speed_r + speed_l) / 2.0;

        // printf(" speed_r: %f, speed_l: %f, speed: %f\n", speed_r, speed_l, speed);  
        // ESP_LOGE(TAG, "speed_r: %f, speed_l: %f, speed: %f", speed_r, speed_l, speed);
        // printf("%f, %f, %f, %f, %f\n", tar_speed, speed, len_mouse, V_r, V_l);
        // ESP_LOGE(TAG, "%f, %f, %f, %f, %f\n", tar_speed, speed, len_mouse, V_r, V_l);
        // ESP_LOGI(TAG, "diff_r=%d, diff_l=%d, spd_r=%f, spd_l=%f", diff_pulse_r, diff_pulse_l, speed_new_r, speed_new_l);

        // 積分項（飽和あり）
        I_speed += speed;
        if (I_speed > 30 * 10000000000) {
            I_speed = 30 * 10000000000;
        } else if (I_speed < -1 * 10000000000) {
            I_speed = -1 * 10000000000;
        }

        // 総移動距離（左右の新速度の平均を足す）
        len_mouse += (speed_new_r + speed_new_l) / 2.0;

        // ESP_LOGE(TAG, "len_mouse: %f", len_mouse);

        // 次回差分計算用に保持
        before_locate_r = locate_r;
        before_locate_l = locate_l;

        state = true; // Move to the next state


    /*****************************************************************************************
     ジャイロ読み取り（角速度）：
        値取得 → ラジアン変換 → 積分 → 角度更新
    *****************************************************************************************/
    if (state == 1) {
        // ジャイロ値更新
        // MPU6500_read_accel_gyro();

        // ローパスフィルタ適用前の値取得
        gyro_x_new = imu_ag.gx_f;
        gyro_x = (gyro_x_new);

        // 角速度の更新
        p_ang_vel = ang_vel;
        ang_vel = gyro_x;

        // 積分項（飽和付き）
        I_ang_vel += ang_vel;
        if (I_ang_vel > 30 * 10000000000) {
            I_ang_vel = 30 * 10000000000;
        } else if (I_ang_vel < -1 * 10000000000) {
                I_ang_vel = -1 * 10000000000;
        }

        int64_t now = esp_timer_get_time(); // μs
        float dt = (now - last_time) / 1000000.0f;
        last_time = now;

        // 角度の更新（度単位）
        // degree += (gyro_x_new + 0.02) * 360 / PI / 100.0;
        degree += ((gyro_x_new)*(180.0 / PI) * dt); // Convert to degrees


        // ESP_LOGE(TAG, "dgree: %f", degree);

        // if(count_LOGE == 10){
        //     // ESP_LOGI(TAG_MPU, "Accel: X=%d Y=%d Z=%d", ax, ay, az);
            // ESP_LOGE(TAG, "gyro_x_new: %f, dgree: %f", gyro_x_new, degree);
        //     ESP_LOGE(TAG, "gyro_x_new: %f, dgree: %f", gyro_x_new, degree);
        //     ESP_LOGE(TAG, "len_mouse: %f", len_mouse);
        //     printf("\x1b[2J");
        //     printf("\x1b[0;0H");
        //     count_LOGE = 0;
        // }else{
        //     count_LOGE++;
        // }
    }    

    // printf("\x1b[2J");
    // printf("\x1b[0;0H");
}