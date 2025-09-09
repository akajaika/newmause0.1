#include "moter_test.h"

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
#include "static_parameters.h"
#include "parameters.h"
#include "glob_ver.h"
#include "typedef.h"
#include "moter_test.h"
#include "MA732_read.h"
#include "cmt_timer.h"
#include "adc.h"
#include "test_ledc.h"

void straight(float len, float acc, float max_sp, float end_sp){
	I_tar_ang_vel = 0;
	I_ang_vel = 0;
	I_tar_speed = 0;
	I_speed = 0;
	//走行モードを直線にする
	run_mode = STRAIGHT_MODE;
	//壁制御を有効にする
	con_wall.enable = true;
	//目標距離をグローバル変数に代入する
	len_target = len;
	//目標速度を設定
	end_speed = end_sp;
	//加速度を設定
	accel = acc;
	//最高速度を設定
	max_speed = max_sp;	

	motor.status = true; // Enable motors

	
	if(end_speed == 0){	//最終的に停止する場合
		//減速処理を始めるべき位置まで加速、定速区間を続行
		while( ((len_target -10) - len_mouse) >  1000.0*((float)(tar_speed * tar_speed) - (float)(end_speed * end_speed))/(float)(2.0*accel)){
			// printf("motors are doing\n\n");
			// printf("while %f > %f\n", ((len_target -10) - len_mouse), 1000.0*((float)(tar_speed * tar_speed) - (float)(end_speed * end_speed))/(float)(2.0*accel));
			// // printf("len_target %f\n", len_target);
			// printf("len_mouse %f\n", len_mouse);
			// printf(" speed_r: %f, speed_l: %f, speed: %f\n", speed_r, speed_l, speed);
			// printf("tar_speed %f\n", tar_speed);
			// // printf("end_speed %f\n", end_speed);
			// // printf("accel %f\n", accel);
			// // printf("max_speed %f\n", max_speed);
			// printf("%f, %f, %f, %f, %f\n", tar_speed, speed, len_mouse, V_r, V_l);

		};
		//減速処理開始
		accel = -acc;					//減速するために加速度を負の値にする	
		while(len_mouse < len_target -1){		//停止したい距離の少し手前まで継続
			// printf("Starting straight test...\n\n");
			// printf("len_mouse %f\n", len_mouse);
			// printf("len_target %f\n", len_target);
			// printf("speed %f\n", speed);
			// printf("%f, %f, %f, %f, %f\n", tar_speed, speed, len_mouse, V_r, V_l);

			//一定速度まで減速したら最低駆動トルクで走行
			// if(tar_speed <= MIN_SPEED){	//目標速度が最低速度になったら、加速度を0にする
			// 	accel = 0;
			// 	tar_speed = MIN_SPEED;
			// }
		}
		printf("end while...\n\n\n\n");
		accel = 0;
		tar_speed = 0;
		//速度が0以下になるまで逆転する
		// while(speed >= 0.0);
		motor.status = false; // Disable motors
			
	}else{
		//減速処理を始めるべき位置まで加速、定速区間を続行
		while( ((len_target-10) - len_mouse) >  1000.0*((float)(tar_speed * tar_speed) - (float)(end_speed * end_speed))/(float)(2.0*accel));
		
		//減速処理開始
		accel = -acc;					//減速するために加速度を負の値にする	
		while(len_mouse < len_target){		//停止したい距離の少し手前まで継続
			//一定速度まで減速したら最低駆動トルクで走行
			if(tar_speed <= end_speed){	//目標速度が最低速度になったら、加速度を0にする
				accel = 0;
				//tar_speed = end_speed;
			}
		}
	}
	//加速度を0にする
	accel = 0;
	//現在距離を0にリセット
	len_mouse = 0;
}


void app_main () {
	motor.status = false; // Enable motors
	
	printf("Before ledc_test_pwm_init\n\n");
	ledc_test_pwm_init(GPIO_NUM_38, GPIO_NUM_17, 0);
	vTaskDelay(pdMS_TO_TICKS(1000));

	printf("Before setup_gpio\n\n");
	gpio_setup();
	vTaskDelay(pdMS_TO_TICKS(1000));

	printf("Before setup_adc\n\n");
	setup_adc();
	vTaskDelay(pdMS_TO_TICKS(1000));

    printf("Before init_moter\n\n");
    init_moter();
    vTaskDelay(pdMS_TO_TICKS(1000));

    printf("Before init_imu\n\n");
    init_imu();
    vTaskDelay(pdMS_TO_TICKS(1000));

    printf("Before setup_cmt_timer\n\n");
    setup_cmt_timer();
    vTaskDelay(pdMS_TO_TICKS(1000));

	// printf("adc reading\n\n");

	// int as = 1;
	// gpio_set_level(GPIO_NUM_5, 1);
	// gpio_set_level(GPIO_NUM_7, 1);
	// gpio_set_level(GPIO_NUM_9, 1); //LED付いてない
	// gpio_set_level(GPIO_NUM_14, 1);

	// while (1)
	// {
	// 	switch (as)
	// 	{
	// 	case 1:
	// 		printf("Doing case 1...\n\n");
	// 		gpio_set_level(GPIO_NUM_14, 1); // LED消灯
	// 		gpio_set_level(GPIO_NUM_5, 0); // LED点灯
	// 		as += 1; // Increment to next case
	// 		break;

	// 	case 2:
	// 		printf("Doing case 2...\n\n");
	// 		gpio_set_level(GPIO_NUM_5, 1); // LED消灯
	// 		gpio_set_level(GPIO_NUM_7, 0); // LED点灯	
	// 		as += 1; // Increment to next case
	// 		break;
	// 	case 3:
	// 		printf("Doing case 3...\n\n");
	// 		gpio_set_level(GPIO_NUM_7, 1); // LED消灯
	// 		gpio_set_level(GPIO_NUM_9, 0); // LED点灯
	// 		as += 1; // Increment to next case
	// 		break;	

	// 	case 4:
	// 		printf("Doing case 4...\n\n");
	// 		gpio_set_level(GPIO_NUM_9, 1); // LED消灯
	// 		gpio_set_level(GPIO_NUM_14, 0); // LED点灯
	// 		as = 1; // Reset to 0 to cycle through cases
	// 		break;
		
	// 	default:
	// 		break;
	// 	}
	// 	vTaskDelay(pdMS_TO_TICKS(2500));
	// 	adc_read(1);
	// 	vTaskDelay(pdMS_TO_TICKS(2500));
	// }
	
		// while (len_mouse < 90){	
		// 	printf("Starting motor test...\n\n");
		// 	vTaskDelay(pdMS_TO_TICKS(1000));
		// 		// printf("Motor test start wait.\n")
		// 	int t = 100;
		// 	for (int i = 0; i < 10; i++) 
		// 	{
		// 		ledc_test_pwm(GPIO_NUM_17, t); // 中間デューティ
		// 		ledc_test_pwm(GPIO_NUM_38, t);
		// 		// printf("Doing motor test %d...\n\n", i);
		// 		// t -= 10; 
		// 		vTaskDelay(pdMS_TO_TICKS(100));
		// 	}
		// 	// ledc_test_pwm(GPIO_NUM_17, 128);  // 低デューティ
		// 	// printf("Doing motor test2...\n\n");
		// 	// vTaskDelay(pdMS_TO_TICKS(1000));
		// 	// printf("Doing motor test3...\n\n");
		// 	ledc_test_pwm(GPIO_NUM_17, 0);   // 停止
			
		// }

		// while (1)
		// {
		// 	ledc_test_pwm(GPIO_NUM_17, 20); // 中間デューティ
		// 	ledc_test_pwm(GPIO_NUM_38, 20);
		// }
		


	// imu();

    // mot_on(250, 250);
	// vTaskDelay(pdMS_TO_TICKS(1000));
	// mot_on(500, 500);
	// mot_on(1, 1);
	// vTaskDelay(pdMS_TO_TICKS(10));

		// vTaskDelay(pdMS_TO_TICKS(1000));
		// printf("Starting straight test...\n\n");
		straight(SECTION*6, 0.3, 0.3, 0.0);
		printf("Finished straight test...\n\n");
	// motor.status = true;
}


