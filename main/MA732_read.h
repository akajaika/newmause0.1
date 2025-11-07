extern void init_imu(void);
extern void MA732_read(void);
extern void MPU6500_read_accel_gyro(void);
extern void imu(void);
extern void MPU6500_read_log(void);

// -----------------------------------------------------------------------------
// MA732GQ-Z: raw14 (0 ～ 16383) → 0.0 ～ 360.0° に変換
// -----------------------------------------------------------------------------
static inline float ma732_raw14_to_degree(uint16_t raw14)
{
    return (float)raw14 * (360.0f / 16383.0f);
}

extern bool enc_update_flag;
extern bool gyro_update_flag;

extern float angle1, angle2;