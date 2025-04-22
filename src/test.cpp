#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "driver/spi_master.h"  
#include "MPU6500.hpp"
#include "PCA9533.hpp"
#include <stdio.h>
#define ESP_ERROR_CHECK(x)

uint8_t data_wr[2] = {0x05, 0x55};


void test(){
    ESP_ERROR_CHECK(i2c_master_transmit(PCA9533_handle, data_wr, 2, -1));
    
    ESP_ERROR_CHECK(spi_device_polling_transmit(dev_handle, &trans));
    printf("IMU: 0x%02X\n", recv_data[0]);

    ESP_ERROR_CHECK(spi_device_polling_transmit(dev1_handle, &trans1));
    ESP_ERROR_CHECK(spi_device_polling_transmit(dev2_handle, &trans2));

    gpio_set_level(GPIO_NUM_5, 1);
    gpio_set_level(GPIO_NUM_7, 1);
    gpio_set_level(GPIO_NUM_9, 1);
    gpio_set_level(GPIO_NUM_14, 1);

    ESP_ERROR_CHECK(adc_continuous_start(adc_config));
}