#include "driver/i2c_master.h"
#define ESP_ERROR_CHECK(x)

i2c_master_bus_config_t i2c_mst_config = {
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .i2c_port = 0,
    .scl_io_num = GPIO_NUM_46,
    .sda_io_num = GPIO_NUM_45,
    .glitch_ignore_cnt = 7,
};

i2c_master_bus_handle_t bus_handle;
ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

i2c_device_config_t PCA9533_cfg = {
    .dev_addr_length = I2C_ADDR_BIT_LEN_7,
    .device_address = 0xC4,
    .scl_speed_hz = 100000,
};

i2c_master_dev_handle_t PCA9533_handle;
ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &PCA9533_cfg, &PCA9533_handle));
