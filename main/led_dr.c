#include "driver/i2c_master.h"
#include "esp_log.h"

i2c_master_dev_handle_t PCA9533_handle;

void PCA9533_init() {
    i2c_master_bus_config_t i2c_mst_config = {
        .i2c_port = 0,
        .sda_io_num = 45,
        .scl_io_num = 46,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
    };
    i2c_master_bus_handle_t bus_handle;
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    i2c_device_config_t PCA9533_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0x62, // A0, A1ピン設定に応じて異なる
        .scl_speed_hz = 100000,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &PCA9533_cfg, &PCA9533_handle));
}

void PCA9533_set_led0_on(int state) {
    uint8_t data[2];
    data[0] = 0x05;  // LS0レジスタ
    data[1] = state;  // LED0=ON, LED1〜3=OFF
    ESP_ERROR_CHECK(i2c_master_transmit(PCA9533_handle, data, 2, -1));
}
