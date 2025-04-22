#include "driver/spi_master.h"

spi_device_interface_config_t dev1_cfg = {
    .clock_speed_hz = 1 * 1000 * 1000, 
    .mode = 0,
    .spics_io_num = IO10,
    .queue_size = 1,
    .flags = 0,
};

spi_device_handle_t dev1_handle;
ESP_ERROR_CHECK(spi_bus_add_device(SPI1_HOST, &dev1_cfg, &dev1_handle));

spi_device_interface_config_t dev2_cfg = {
    .clock_speed_hz = 1 * 1000 * 1000, 
    .mode = 0,
    .spics_io_num = IO8,
    .queue_size = 1,
    .flags = 0,
};

spi_device_handle_t dev2_handle;
ESP_ERROR_CHECK(spi_bus_add_device(SPI1_HOST, &dev2_cfg, &dev2_handle));

uint8_t send_data = 0x55;
uint8_t recv_data[1];

spi_transaction_t trans1 = {
  .length = 8,
  .tx_buffer = &send_data,
  .rx_buffer = &recv_data,
};

spi_transaction_t trans2 = {
    .length = 8,
    .tx_buffer = &send_data,
    .rx_buffer = &recv_data,
};

ESP_ERROR_CHECK(spi_device_polling_transmit(dev1_handle, &trans1));
ESP_ERROR_CHECK(spi_device_polling_transmit(dev2_handle, &trans2));