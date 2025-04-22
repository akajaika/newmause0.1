
uint8_t data_wr[DATA_LENGTH] = {0x05, 0x55};


void test(){
    ESP_ERROR_CHECK(i2c_master_transmit(PCA9533_handle, data_wr, 2, -1));
    std::cout << "IMU " << static_cast<int>(recv_data) << std::endl;

    gpio_set_level(GPIO_NUM_5, 1);
    gpio_set_level(GPIO_NUM_7, 1);
    gpio_set_level(GPIO_NUM_9, 1);
    gpio_set_level(GPIO_NUM_14, 1);

    ESP_ERROR_CHECK(adc_continuous_start(adc_config));
}