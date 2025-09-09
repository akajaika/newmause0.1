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
#include "driver/i2c_master.h"
#include "typedef.h"
#include "MA732_read.h"

// ログ用タグ
#define TAG_MA732 "MA732"
#define TAG_MPU   "MPU6500"
#define TAG_I2C   "I2C"

// 角度レジスタのアドレス（MSB first, Read = 1）
#define MA732_ANG_ADDR  0x3FFE

// SPIデバイスハンドル
spi_device_handle_t dev1_handle;  // MA732 右（CS = GPIO10）
spi_device_handle_t dev2_handle;  // MA732 左（CS = GPIO8）
spi_device_handle_t dev_handle;   // MPU6500 用（CS = GPIO37）

// I2Cデバイスハンドル
i2c_master_dev_handle_t PCA9533_handle;

// SPI通信データ（MPU6500 用）
uint8_t send_data_enc1 = 0x55;
uint8_t send_data_enc2 = 0x55;
uint8_t send_data_mpu  = 0x55;
uint8_t recv_data_mpu[1];

// MPU6500 用 SPI トランザクション
spi_transaction_t trans;

uint16_t raw1, raw2; 
int16_t ax, ay, az, gx, gy, gz;

float angle1 = 0.0f, angle2 = 0.0f; // MA732 エンコーダ角度（度単位）


// -----------------------------------------------------------------------------
// SPI バス初期化：SPI2_HOST (MPU6500)、SPI3_HOST (MA732×2)
// -----------------------------------------------------------------------------
void setup_spi()
{
    // SPI2_HOST を MPU6500 用に初期化 (MOSI=35, MISO=34, SCLK=36)
    spi_bus_config_t spi2_cfg = {
        .mosi_io_num = 35,
        .miso_io_num = 34,
        .sclk_io_num = 36,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &spi2_cfg, SPI_DMA_DISABLED));

    // SPI3_HOST を MA732 用に初期化 (MOSI=11, MISO=13, SCLK=12)
    spi_bus_config_t spi3_cfg = {
        .mosi_io_num = 11,
        .miso_io_num = 13,
        .sclk_io_num = 12,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(SPI3_HOST, &spi3_cfg, SPI_DMA_DISABLED));
}

// -----------------------------------------------------------------------------
// MPU6500: 1バイト書き込み関数
// -----------------------------------------------------------------------------
esp_err_t mpu6500_write_byte(uint8_t reg_addr, uint8_t data)
{
    uint8_t tx_data[2] = { reg_addr & 0x7F, data };  // MSB=0 で書き込み
    spi_transaction_t t = {
        .length    = 8 * 2,
        .tx_buffer = tx_data,
        .rx_buffer = NULL,
    };
    return spi_device_polling_transmit(dev_handle, &t);
}

// -----------------------------------------------------------------------------
// MPU6500: 複数バイト読み出し関数
// -----------------------------------------------------------------------------
esp_err_t mpu6500_read_bytes(uint8_t reg_addr, int8_t *data, size_t len)
{
    uint8_t tx = reg_addr | 0x80;  // MSB=1 で読み出し
    spi_transaction_t t = {
        .length    = 8 * (1 + len),
        .tx_buffer = &tx,
        .rxlength  = 8 * (1 + len),
        .rx_buffer = malloc(len + 1),
    };
    if (!t.rx_buffer) {
        return ESP_ERR_NO_MEM;
    }
    esp_err_t ret = spi_device_polling_transmit(dev_handle, &t);
    if (ret == ESP_OK) {
        memcpy(data, ((int8_t*)t.rx_buffer) + 1, len);  // 先頭1バイトはダミー
    }
    free(t.rx_buffer);
    return ret;
}

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
        .tx_buffer = tx_addr,
        .rxlength  = 0,
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
        .tx_buffer = tx_dummy,
        .rxlength  = 16,
        .rx_buffer = rx_data,
    };
    ret = spi_device_polling_transmit(spi_hdl, &t2);
    if (ret != ESP_OK) {
        return ret;
    }

    // 生データを 16bit 結合し、下位14bit を抽出
    uint16_t raw14 = ((uint16_t)rx_data[0] << 8) | rx_data[1];
    raw14 = raw14& 0xFFFC;
    *out_raw14 = raw14 >> 2;
    return ESP_OK;
}

// -----------------------------------------------------------------------------
// MA732GQ-Z 右 (CS=GPIO10) 初期化
// -----------------------------------------------------------------------------
void MA732_init_right(void)
{
    spi_device_interface_config_t cfg = {
        .clock_speed_hz = 1000 * 1000,  // 1 MHz
        .mode           = 0,           // CPOL=0, CPHA=0
        .spics_io_num   = 10,          // CS = GPIO10
        .queue_size     = 1,
        .flags          = 0,
    };
    ESP_ERROR_CHECK(spi_bus_add_device(SPI3_HOST, &cfg, &dev1_handle));
}

// -----------------------------------------------------------------------------
// MA732GQ-Z 左 (CS=GPIO8) 初期化
// -----------------------------------------------------------------------------
void MA732_init_left(void)
{
    spi_device_interface_config_t cfg = {
        .clock_speed_hz = 1000 * 1000,  // 1 MHz
        .mode           = 0,
        .spics_io_num   = 8,           // CS = GPIO8
        .queue_size     = 1,
        .flags          = 0,
    };
    ESP_ERROR_CHECK(spi_bus_add_device(SPI3_HOST, &cfg, &dev2_handle));
}

// -----------------------------------------------------------------------------
// MPU6500 初期化
// -----------------------------------------------------------------------------
void MPU6500_init(void)
{
    spi_device_interface_config_t dev_cfg = {
        .clock_speed_hz = 1 * 1000 * 1000,
        .mode           = 0,
        .spics_io_num   = 37,
        .queue_size     = 1,
        .flags          = 0,
    };
    ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &dev_cfg, &dev_handle));

    memset(&trans, 0, sizeof(trans));
    trans.length    = 8;
    trans.tx_buffer = &send_data_mpu;
    trans.rx_buffer = recv_data_mpu;
}

// -----------------------------------------------------------------------------
// MPU6500 設定
// -----------------------------------------------------------------------------
void MPU6500_configure(void)
{
    mpu6500_write_byte(0x6B, 0x00); // PWR_MGMT_1
    vTaskDelay(pdMS_TO_TICKS(100));
    mpu6500_write_byte(0x19, 0x07); // SMPLRT_DIV
    mpu6500_write_byte(0x1A, 0x06); // CONFIG
    mpu6500_write_byte(0x1B, 0x18); // GYRO_CONFIG
    mpu6500_write_byte(0x1C, 0x10); // ACCEL_CONFIG
    mpu6500_write_byte(0x1D, 0x03); // ACCEL_CONFIG2
}

// -----------------------------------------------------------------------------
// MPU6500 加速度・ジャイロ読み出し（サンプル）
// -----------------------------------------------------------------------------
void MPU6500_read_accel_gyro(void)
{
    int8_t raw_data[14];
    mpu6500_read_bytes(0x3B, raw_data, 14);

    ax = (raw_data[0] << 8) | raw_data[1];
    ay = (raw_data[2] << 8) | raw_data[3];
    az = (raw_data[4] << 8) | raw_data[5];

    gx = (raw_data[8] << 8) | raw_data[9];
    gy = (raw_data[10] << 8) | raw_data[11];
    gz = (raw_data[12] << 8) | raw_data[13];

    imu_ag.ax_f = (2*(float)ax / 4096.0f);
    imu_ag.ay_f = (2*(float)ay / 4096.0f);
    imu_ag.az_f = (2*(float)az / 4096.0f);
    imu_ag.gx_f = (2000.0*(float)gx / 32767.0f)*3.1415/180.0;
    imu_ag.gy_f = (2000.0*(float)gy / 32767.0f)*3.1415/180.0;    
    imu_ag.gz_f = (2000.0*(float)gz / 32767.0f)*3.1415/180.0;

    // ESP_LOGI(TAG_MPU, "Accel: X=%d Y=%d Z=%d", ax, ay, az);
    // ESP_LOGI(TAG_MPU, "Gyro : X=%d Y=%d Z=%d", gx, gy, gz);
}

// -----------------------------------------------------------------------------
// MA732GQ-Z から右・左両方の角度を読み出して表示
// -----------------------------------------------------------------------------
void MA732_read(void)
{

    // 右エンコーダ読取 (CS=GPIO10)
    if (ma732_read_raw14(dev1_handle, &raw1) == ESP_OK) {
        // angle1 = ma732_raw14_to_degree(raw1)/2.0f; // 2倍して 0.0 ～ 360.0° に変換
        angle1 = raw1; // 生データのまま取得
    } else {
        ESP_LOGE(TAG_MA732, "右 MA732 読み出し失敗");
        angle1 = -1.0f;
    }

    // 左エンコーダ読取 (CS=GPIO8)
    if (ma732_read_raw14(dev2_handle, &raw2) == ESP_OK) {
        // angle2 = ma732_raw14_to_degree(raw2)/2.0f; // 2倍して 0.0 ～ 360.0° に変換
        angle2 = raw2; // 生データのまま取得
    } else {
        ESP_LOGE(TAG_MA732, "左 MA732 読み出し失敗");
        angle2 = -1.0f;
    }
}

// -----------------------------------------------------------------------------
// アプリケーションメイン
// -----------------------------------------------------------------------------
void imu(void)
{
    // // 1) SPI バス初期化
    // setup_spi();

    // // 2) 各デバイス登録
    // MPU6500_init();
    // MA732_init_right();
    // MA732_init_left();

    // // 3) MPU6500 設定
    // MPU6500_configure();

    

    // printf("Accel: X Y Z\nGyro : X Y Z\n");

    // 4) メインループ
    while (1) {
        MPU6500_read_accel_gyro();
        MA732_read();
        printf("MA732: Right=%.2f Left=%.2f\n", angle1, angle2);
        vTaskDelay(pdMS_TO_TICKS(100));
        //     // 結果をターミナルに表示（画面クリア＆カーソル移動はお好みで）
        // printf("%f,%f,%f,%f,%f,%f\n",
        //        imu_ag.ax_f, imu_ag.ay_f, imu_ag.az_f, imu_ag.gx_f, imu_ag.gy_f, imu_ag.gz_f);
        printf("\x1b[2J");
        printf("\x1b[0;0H");
    }
}

void init_imu(void)
{
    // 1) SPI バス初期化
    setup_spi();

    // 2) 各デバイス登録
    MPU6500_init();
    MA732_init_right();
    MA732_init_left();

    // 3) MPU6500 設定
    MPU6500_configure();
    
    // DMA を使わないシンプルな最初の読み出し
    ESP_ERROR_CHECK(spi_device_polling_transmit(dev_handle, &trans));
    ESP_LOGI(TAG_MPU, "IMU: 0x%02X", recv_data_mpu[0]);
}
