/**
 * @file lsm6ds3tr.c
 * @brief LSM6DS3TR-C I2C driver implementation for Atmega328 using IIC_ functions
 * @author Nate Hunter
 * @date 2025-07-14
 * @version v2.1.0
 */

#include "lsm6ds3tr.h"
#include "IICFuncs.h"

/**
 * @brief Initialize the LSM6DS3 device with desired configuration
 * 
 * @param dev Pointer to LSM6DS3 device handle
 * @param accelFS Accelerometer full-scale range
 * @param gyroFS Gyroscope full-scale range
 * @return 1 on success, 0 on failure
 */
uint8_t LSM6DS3_Init(LSM6DS3_Handle *dev,
                     LSM6DS3_AccelFS accelFS,
                     LSM6DS3_GyroFS gyroFS) {
  uint8_t rx;

  if (IIC_ReadByte(dev->i2c_addr, LSM6DS3_REG_WHO_AM_I, &rx) != IIC_SUCCESS || (rx != LSM6DS3_WHO_AM_I && rx != LSM6DS3TR_WHO_AM_I))
    return 0;

  if (IIC_WriteByte(dev->i2c_addr, LSM6DS3_REG_CTRL1_XL, (dev->accelODR << 4) | (accelFS << 2)) != IIC_SUCCESS)
    return 0;

  if (IIC_WriteByte(dev->i2c_addr, LSM6DS3_REG_CTRL2_G, (dev->gyroODR << 4) | (gyroFS << 1)) != IIC_SUCCESS)
    return 0;

  if (IIC_WriteByte(dev->i2c_addr, LSM6DS3_REG_CTRL3_C, 0x44) != IIC_SUCCESS)
    return 0;

  // Set accelerometer scale factor
  switch (accelFS) {
    case LSM6DS3_XL_2G:
      dev->accelScale = 0.061f / 1000;
      break;
    case LSM6DS3_XL_4G:
      dev->accelScale = 0.122f / 1000;
      break;
    case LSM6DS3_XL_8G:
      dev->accelScale = 0.244f / 1000;
      break;
    case LSM6DS3_XL_16G:
      dev->accelScale = 0.488f / 1000;
      break;
    default:
      return 0;
  }

  // Set gyroscope scale factor
  switch (gyroFS) {
    case LSM6DS3_GYRO_125DPS:
      dev->gyroScale = 4.375f / 1000;
      break;
    case LSM6DS3_GYRO_250DPS:
      dev->gyroScale = 8.75f / 1000;
      break;
    case LSM6DS3_GYRO_500DPS:
      dev->gyroScale = 17.5f / 1000;
      break;
    case LSM6DS3_GYRO_1000DPS:
      dev->gyroScale = 35.0f / 1000;
      break;
    case LSM6DS3_GYRO_2000DPS:
      dev->gyroScale = 70.0f / 1000;
      break;
    default:
      return 0;
  }

  return 1;
}

/**
 * @brief Read accelerometer and gyroscope data from LSM6DS3
 * 
 * @param dev Pointer to LSM6DS3 device handle
 * @param accel Output array for accelerometer values (X, Y, Z)
 * @param gyro Output array for gyroscope values (X, Y, Z)
 * @return 1 on success, 0 on failure
 */
uint8_t LSM6DS3_ReadData(LSM6DS3_Handle *dev, float accel[3], float gyro[3]) {
  uint8_t buffer[12];
  if (IIC_ReadBytes(dev->i2c_addr, LSM6DS3_REG_OUTX_L_G, buffer, 12) != IIC_SUCCESS)
    return 0;

  // Gyroscope: X, Y, Z
  for (int i = 0; i < 3; i++) {
    int16_t raw = (int16_t)(buffer[i * 2 + 1] << 8 | buffer[i * 2]);
    gyro[i] = raw * dev->gyroScale;
  }

  // Accelerometer: X, Y, Z
  for (int i = 0; i < 3; i++) {
    int16_t raw = (int16_t)(buffer[6 + i * 2 + 1] << 8 | buffer[6 + i * 2]);
    accel[i] = raw * dev->accelScale;
  }

  return 1;
}
