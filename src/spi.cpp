#include "driver/spi_master.h"
#define ESP_ERROR_CHECK(x)

spi_bus_config_t spi1_bus_config = {
    .mosi_io_num = 11,
    .miso_io_num = 13,
    .sclk_io_num = 12,
    .quadwp_io_num = -1, 
    .quadhd_io_num = -1,  
};

ESP_ERROR_CHECK(spi_bus_initialize(SPI1_HOST, &spi1_bus_config, SPI_DMA_DISABLED));

spi_bus_config_t spi2_bus_config = {
    .mosi_io_num = 35,
    .miso_io_num = 34,
    .sclk_io_num = 36,
    .quadwp_io_num = -1, 
    .quadhd_io_num = -1,  
};

ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &spi2_bus_config, SPI_DMA_DISABLED));