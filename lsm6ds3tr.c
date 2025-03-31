/**
 * @file lsm6ds3tr.c
 * @brief LSM6DS3TR-C accelerometer and gyroscope driver implementation
 */

#include "lsm6ds3tr.h"
#include "IICFuncs.h"  // I2C library for ATmega328

// Private function prototypes
static bool lsm6_verify_device(uint8_t i2c_addr);
static bool lsm6_write_reg(uint8_t i2c_addr, uint8_t reg, uint8_t value);
static bool lsm6_read_reg(uint8_t i2c_addr, uint8_t reg, uint8_t *value);
static bool lsm6_read_regs(uint8_t i2c_addr, uint8_t reg, uint8_t *data, uint8_t len);

/**
 * @brief Initialize LSM6 sensor
 */
bool lsm6_init(lsm6_config_t *config) {
  if (!lsm6_verify_device(config->i2c_addr)) {
    return false;
  }

  // Configure accelerometer
  uint8_t ctrl1_xl = (config->accel_odr << 4) | (config->accel_fs << 2);
  if (config->accel_high_performance) {
    ctrl1_xl &= ~(1 << 1);  // XL_HM_MODE = 0
  } else {
    ctrl1_xl |= (1 << 1);  // XL_HM_MODE = 1
  }
  if (!lsm6_write_reg(config->i2c_addr, LSM6_REG_CTRL1_XL, ctrl1_xl)) {
    return false;
  }

  // Configure gyroscope
  uint8_t ctrl2_g = (config->gyro_odr << 4);
  if (config->gyro_fs == LSM6_GYRO_FS_125DPS) {
    ctrl2_g |= (1 << 1);  // FS_125 = 1
  } else {
    ctrl2_g |= (config->gyro_fs << 2);
  }
  if (!lsm6_write_reg(config->i2c_addr, LSM6_REG_CTRL2_G, ctrl2_g)) {
    return false;
  }

  return true;
}

/**
 * @brief Read raw sensor data
 */
bool lsm6_read_raw(lsm6_data_t *data, lsm6_config_t *config) {
  uint8_t buffer[12];

  // Read accelerometer data
  if (!lsm6_read_regs(config->i2c_addr, LSM6_REG_OUTX_L_XL, buffer, 6)) {
    return false;
  }
  data->accel_x = (int16_t)((buffer[1] << 8) | buffer[0]);
  data->accel_y = (int16_t)((buffer[3] << 8) | buffer[2]);
  data->accel_z = (int16_t)((buffer[5] << 8) | buffer[4]);

  // Read gyroscope data
  if (!lsm6_read_regs(config->i2c_addr, LSM6_REG_OUTX_L_G, buffer, 6)) {
    return false;
  }
  data->gyro_x = (int16_t)((buffer[1] << 8) | buffer[0]);
  data->gyro_y = (int16_t)((buffer[3] << 8) | buffer[2]);
  data->gyro_z = (int16_t)((buffer[5] << 8) | buffer[4]);

  return true;
}

/**
 * @brief Convert raw accelerometer data to g's
 */
float lsm6_accel_to_g(int16_t raw, lsm6_data_t *data) {
  return (float)raw * data->accel_scale;
}

/**
 * @brief Convert raw gyroscope data to dps
 */
float lsm6_gyro_to_dps(int16_t raw, lsm6_data_t *data) {
  return (float)raw * data->gyro_scale;
}

// Private functions

static bool lsm6_verify_device(uint8_t i2c_addr) {
  uint8_t who_am_i;
  if (!lsm6_read_reg(i2c_addr, LSM6_REG_WHO_AM_I, &who_am_i)) {
    return false;
  }
  return (who_am_i == 0x6A);
}

static bool lsm6_write_reg(uint8_t i2c_addr, uint8_t reg, uint8_t value) {
  uint8_t data[2] = { reg, value };
  return i2c_write(i2c_addr, data, 2);
}

static bool lsm6_read_reg(uint8_t i2c_addr, uint8_t reg, uint8_t *value) {
  return i2c_write_read(i2c_addr, &reg, 1, value, 1);
}

static bool lsm6_read_regs(uint8_t i2c_addr, uint8_t reg, uint8_t *data, uint8_t len) {
  return i2c_write_read(i2c_addr, &reg, 1, data, len);
}