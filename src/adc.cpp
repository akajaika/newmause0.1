#include "esp_adc/adc_continuous.h"
#define ESP_ERROR_CHECK(x)

void app_main() {
    adc_continuous_handle_t handle = NULL;

    adc_continuous_handle_cfg_t adc_config = {
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

    ESP_ERROR_CHECK(adc_continuous_new_handle(&adc_config, &handle));
}