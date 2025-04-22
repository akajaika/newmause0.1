#include "driver/spi_master.h"
#define ESP_ERROR_CHECK(x)

spi_device_handle_t dev_handle;
spi_transaction_t trans;
uint8_t recv_data[1];

void MPU6500_init() {
  spi_device_interface_config_t dev_cfg = {
      .clock_speed_hz = 1 * 1000 * 1000, 
      .mode           = 0,
      .spics_io_num   = 37,
      .queue_size     = 1,
      .flags          = 0,
  };

  ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &dev_cfg, &dev_handle));

  uint8_t send_data = 0x55;

  spi_transaction_t trans = {
    .length     = 8,
    .tx_buffer  = &send_data,
    .rx_buffer  = &recv_data,
  };
}
  
void app_main(){
  MPU6500_init();
}