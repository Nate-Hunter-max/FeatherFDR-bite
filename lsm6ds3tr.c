/**
 * @file lsm6ds3tr.c
 * @brief LSM6DS3TR-C accelerometer and gyroscope driver implementation
 */

#include "lsm6ds3tr.h"
#include "IICFuncs.h"  // I2C library for ATmega328

// Private function prototypes
static bool lsm6_write_reg(uint8_t i2c_addr, uint8_t reg, uint8_t value);
static bool lsm6_read_reg(uint8_t i2c_addr, uint8_t reg, uint8_t *value);
static bool lsm6_read_regs(uint8_t i2c_addr, uint8_t reg, uint8_t *data, uint8_t len);

/**
 * @brief Initialize LSM6 sensor
 * 
 * @param sens Pointer to sensor structure to init
 * @return bool True if initialization successful
 */
bool LSM_Init(LSM6_HandleTypeDef *sens) {
    printf("ver...0x%X...",IIC_ReadByte(sens->config.i2c_addr, LSM6_REG_WHO_AM_I)); // TODO: 69 in LSM6_REG_WHO_AM_I?
  if (!(IIC_ReadByte(sens->config.i2c_addr, LSM6_REG_WHO_AM_I) == 0x6A)) {
    return false;
  }

  // Configure accelerometer
  uint8_t ctrl1_xl = (sens->config.accel_odr << 4) | (sens->config.accel_fs << 2);
  if (sens->config.accel_high_performance) {
    ctrl1_xl &= ~(1 << 1);  // XL_HM_MODE = 0
  } else {
    ctrl1_xl |= (1 << 1);  // XL_HM_MODE = 1
  }
  if (!lsm6_write_reg(sens->config.i2c_addr, LSM6_REG_CTRL1_XL, ctrl1_xl)) {
    return false;
  }

  // Configure gyroscope
  uint8_t ctrl2_g = (sens->config.gyro_odr << 4);
  if (sens->config.gyro_fs == LSM6_GYRO_FS_125DPS) {
    ctrl2_g |= (1 << 1);  // FS_125 = 1
  } else {
    ctrl2_g |= (sens->config.gyro_fs << 2);
  }
  if (!lsm6_write_reg(sens->config.i2c_addr, LSM6_REG_CTRL2_G, ctrl2_g)) {
    return false;
  }

  return true;
}

/**
 * @brief Read raw sensor data
 * 
 * @param sens Pointer to data structure to store results
 * @return bool True if read successful
 */
bool LSM6_Read_raw(LSM6_HandleTypeDef *sens) {
  uint8_t buffer[12];

  // Read accelerometer data
  if (!lsm6_read_regs(sens->config.i2c_addr, LSM6_REG_OUTX_L_XL, buffer, 6)) {
    return false;
  }
  sens->accel[0] = (int16_t)((buffer[1] << 8) | buffer[0]);
  sens->accel[1] = (int16_t)((buffer[3] << 8) | buffer[2]);
  sens->accel[2] = (int16_t)((buffer[5] << 8) | buffer[4]);

  // Read gyroscope data
  if (!lsm6_read_regs(sens->config.i2c_addr, LSM6_REG_OUTX_L_G, buffer, 6)) {
    return false;
  }
  sens->gyro[0] = (int16_t)((buffer[1] << 8) | buffer[0]);
  sens->gyro[1] = (int16_t)((buffer[3] << 8) | buffer[2]);
  sens->gyro[2] = (int16_t)((buffer[5] << 8) | buffer[4]);

  return true;
}

/**
 * @brief Convert raw accelerometer data to g's
 * 
 * @param raw Raw sensor value
 * @param sens Pointer to data structure containing scale factor
 * @return float Acceleration in g's
 */
float LSM6_Accel_to_g(int16_t raw, LSM6_HandleTypeDef *sens) {
  return (float)raw * 1;
}

/**
 * @brief Convert raw gyroscope data to dps
 * 
 * @param raw Raw sensor value
 * @param sens Pointer to data structure containing scale factor
 * @return float Angular rate in dps
 */
float LSM6_Gyro_to_dps(int16_t raw, LSM6_HandleTypeDef *sens) {
  return (float)raw * 1;
}

// Private functions

static bool lsm6_write_reg(uint8_t i2c_addr, uint8_t reg, uint8_t value) {
  IIC_WriteByte(i2c_addr, reg, value);
  return 1;
}

static bool lsm6_read_reg(uint8_t i2c_addr, uint8_t reg, uint8_t *value) {
  return IIC_ReadByte(i2c_addr, reg);
}

static bool lsm6_read_regs(uint8_t i2c_addr, uint8_t reg, uint8_t *data, uint8_t len) {
  IIC_ReadBytes(i2c_addr, reg, data, len);
  return 1;
}