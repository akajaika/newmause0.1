#ifndef MPU6500_HPP
#define MPU6500_HPP

#include "driver/spi_master.h"
#include <stdint.h>

extern spi_device_handle_t dev_handle;
extern spi_transaction_t trans;
extern uint8_t recv_data[1];

void MPU6500_init(void);

#endif // MPU6500_HPP
