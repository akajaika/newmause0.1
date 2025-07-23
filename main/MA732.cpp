#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "esp_adc/adc_continuous.h"
#include "esp_log.h"
#include "esp_err.h"
#include "driver/spi_master.h"
#include "glob_ver.hpp"

// 角度レジスタのアドレス（MSB first, Read = 1）
#define MA732_ANG_ADDR  0x3FFE
#define TAG_MA732 "MA732"

// SPIデバイスハンドル
spi_device_handle_t dev1_handle;  // MA732 右（CS = GPIO10）
spi_device_handle_t dev2_handle;  // MA732 左（CS = GPIO8）

// SPI通信データ
uint8_t send_data_enc1 = 0x55;
uint8_t send_data_enc2 = 0x55;

uint16_t raw1, raw2; 

// -----------------------------------------------------------------------------
// MA732GQ-Z: 14bit 生データ読み出し (2段階 SPI)
//    spi_hdl = dev1_handle (右, CS=GPIO10) or dev2_handle (左, CS=GPIO8)
// -----------------------------------------------------------------------------
esp_err_t ma732_read_raw14(spi_device_handle_t spi_hdl, uint16_t *out_raw14)
{
    esp_err_t ret;
    uint8_t tx_addr[2];
    uint8_t rx_data[2];

    // --- Step 1: レジスタアドレス送信 (0x3FFE | Read=1)
    tx_addr[0] = ((MA732_ANG_ADDR >> 8) & 0x7F) | 0x80;
    tx_addr[1] = (uint8_t)(MA732_ANG_ADDR & 0xFF);

    spi_transaction_t t1 = {
        .length    = 16,
        .rxlength  = 0,
        .tx_buffer = tx_addr,
        .rx_buffer = NULL,
    };
    ret = spi_device_polling_transmit(spi_hdl, &t1);
    if (ret != ESP_OK) {
        return ret;
    }

    // --- Step 2: ダミー送信して 16bit 実データ受信
    uint8_t tx_dummy[2] = { 0x00, 0x00 };
    spi_transaction_t t2 = {
        .length    = 16,
        .rxlength  = 16,
        .tx_buffer = tx_dummy,
        .rx_buffer = rx_data,
    };
    ret = spi_device_polling_transmit(spi_hdl, &t2);
    if (ret != ESP_OK) {
        return ret;
    }

    // 生データを 16bit 結合し、下位14bit を抽出
    uint16_t raw14 = ((uint16_t)rx_data[0] << 8) | rx_data[1];
    *out_raw14 = raw14& 0xFFFC;
    raw14 = raw14 >> 2;
    return ESP_OK;
}

// -----------------------------------------------------------------------------
// MA732GQ-Z: raw14 (0 ～ 16383) → 0.0 ～ 360.0° に変換
// -----------------------------------------------------------------------------
static inline float ma732_raw14_to_degree(uint16_t raw14)
{
    return (float)raw14 * (360.0f / 32768.0f);
}

// -----------------------------------------------------------------------------
// MA732GQ-Z 右 (CS=GPIO10) 初期化
// -----------------------------------------------------------------------------
void MA732_init_right(void)
{
    spi_device_interface_config_t cfg = {
        .mode           = 0,   
        .clock_speed_hz = 1000 * 1000,  // 1 MHz
        .spics_io_num   = 10,  
        .flags          = 0,        // CPOL=0, CPHA=0        // CS = GPIO10
        .queue_size     = 1,
    };
    ESP_ERROR_CHECK(spi_bus_add_device(SPI3_HOST, &cfg, &dev1_handle));
}

// -----------------------------------------------------------------------------
// MA732GQ-Z 左 (CS=GPIO8) 初期化
// -----------------------------------------------------------------------------
void MA732_init_left(void)
{
    spi_device_interface_config_t cfg = {
        .mode           = 0,
        .clock_speed_hz = 1000 * 1000,  // 1 MHz
        .spics_io_num   = 8,           // CS = GPIO8
        .flags          = 0,
        .queue_size     = 1,
    };
    ESP_ERROR_CHECK(spi_bus_add_device(SPI3_HOST, &cfg, &dev2_handle));
}

// -----------------------------------------------------------------------------
// MA732GQ-Z から右・左両方の角度を読み出して表示
// -----------------------------------------------------------------------------
void MA732_read(void)
{

    // 右エンコーダ読取 (CS=GPIO10)
    if (ma732_read_raw14(dev1_handle, &raw1) == ESP_OK) {
        angle_r = ma732_raw14_to_degree(raw1);
    } else {
        ESP_LOGE(TAG_MA732, "右 MA732 読み出し失敗");
        angle_r = -1.0f;
    }

    // 左エンコーダ読取 (CS=GPIO8)
    if (ma732_read_raw14(dev2_handle, &raw2) == ESP_OK) {
        angle_l = ma732_raw14_to_degree(raw2);
    } else {
        ESP_LOGE(TAG_MA732, "左 MA732 読み出し失敗");
        angle_l = -1.0f;
    }
}

void app_main(){
    MA732_init_right();
    MA732_init_left();
}

