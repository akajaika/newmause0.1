#include "esp_adc/adc_continuous.h"
#include <stdio.h>
#include <inttypes.h>
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_err.h"
#include <string.h>
#include "driver/gpio.h"
#include "parameters.h"
#include "glob_ver.h"
#include "static_parameters.h"
#include "typedef.h"


int output = 1; // GPIOの出力状態を示す変数
static const char *TAG = "adc_example";
adc_continuous_handle_t handle = NULL;


void setup_adc() {
    // ローカル変数は削除し、グローバルのhandleを使う
    adc_continuous_handle_cfg_t adc_handle_cfg = {
        .max_store_buf_size = 1024,
        .conv_frame_size = 256,
    };
    adc_digi_pattern_config_t pattern[5] = {
        {
            .atten = 11,
            .channel = 0,
            .unit = 0,
            .bit_width = 12
        },
        {
            .atten = 11,
            .channel = 1,
            .unit = 0,
            .bit_width = 12
        },
        {
            .atten = 11,
            .channel = 2,
            .unit = 0,
            .bit_width = 12
        },
        {
            .atten = 11,
            .channel = 3,
            .unit = 0,
            .bit_width = 12
        },
        {
            .atten = 11,
            .channel = 5,   
            .unit = 0,
            .bit_width = 12
        }
    };
    adc_continuous_config_t adc_config = {
        .pattern_num = 5,
        .adc_pattern = pattern,
        .sample_freq_hz = 20 * 1000, // 20kHzなど
        .conv_mode = ADC_CONV_SINGLE_UNIT_1,
        .format = ADC_DIGI_OUTPUT_FORMAT_TYPE2,
    };
    ESP_ERROR_CHECK(adc_continuous_new_handle(&adc_handle_cfg, &handle));
    ESP_ERROR_CHECK(adc_continuous_config(handle, &adc_config));
    ESP_ERROR_CHECK(adc_continuous_start(handle)); // 連続変換開始
}

void gpio_setup() {
    // 入力ピンの設定
    gpio_set_direction(GPIO_NUM_1, GPIO_MODE_INPUT);
    gpio_set_direction(GPIO_NUM_2, GPIO_MODE_INPUT);
    gpio_set_direction(GPIO_NUM_3, GPIO_MODE_INPUT);
    gpio_set_direction(GPIO_NUM_4, GPIO_MODE_INPUT);
    //電源監視
    gpio_set_direction(GPIO_NUM_6, GPIO_MODE_INPUT);
    // 出力ピンの設定
    gpio_set_direction(GPIO_NUM_5, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_NUM_7, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_NUM_9, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_NUM_14, GPIO_MODE_OUTPUT);

    gpio_set_direction(GPIO_NUM_15, GPIO_MODE_OUTPUT);
}

int adc_read(int num) {
    uint8_t result[1024] = {0};
    uint32_t ret_num = 0;
    esp_err_t ret = adc_continuous_read(handle, result, sizeof(result), &ret_num, 1000);
  
    
    if (ret == ESP_OK) {
        // printf("ADC Read \n");
        int values[5] = {0};
        for (size_t i = 0; i < ret_num;) {
            adc_digi_output_data_t *p = (adc_digi_output_data_t *)(result + i);
            switch (p->type2.channel) {
                case 0: values[0] = p->type2.data; break;
                case 1: values[1] = p->type2.data; break;
                case 2: values[2] = p->type2.data; break;
                case 3: values[3] = p->type2.data; break;
                case 5: values[4] = p->type2.data; break;
            }
            i += sizeof(adc_digi_output_data_t);
        }
        // ESP_LOGI(TAG, "CHnum:%4d \n",values[num]);
        // ESP_LOGI(TAG, "CH0:%4d CH1:%4d CH2:%4d CH3:%4d CH5:%f \n",
        //         values[0], values[1], values[2], values[3], values[4]*1.067/4095*4.87);
        if (num >= 0 && num < 5) {
            // printf("CH0 value: %d CH1 value: %d CH2 value: %d CH3 value: %d\n", values[0], values[1], values[2], values[3]);
            // printf("CH%d value: %d\n", num, values[num]);
            // printf("\x1b[2J");
            // printf("\x1b[0;0H");
            return values[num]; // numが0-4の範囲内なら対応する値を返す
        } else {
            printf("Invalid channel number: %d\n", num);
            return -1; // 無効なチャンネル番号
        }
        // ESP_LOGE(TAG, "ADC Read: Channel %d, Value: %d", num, values[num]);

    } else if (ret == ESP_ERR_TIMEOUT) {
        ESP_LOGW(TAG, "ADC read timeout");
    } else {
        ESP_LOGE(TAG, "ADC read failed");
    }

    return -1; // エラー時は-1を返す
}
