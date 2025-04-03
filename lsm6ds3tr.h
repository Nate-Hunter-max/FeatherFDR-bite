/**
 * @file lsm6ds3tr.h
 * @brief LSM6DS3TR-C accelerometer and gyroscope driver for ATmega328
 */

#ifndef LSM6DS3TR_H
#define LSM6DS3TR_H

#include <stdint.h>
#include <stdbool.h>

// I2C address (SA0 pin dependent)
#define LSM6_I2C_ADDR_SA0_LOW 0x6A
#define LSM6_I2C_ADDR_SA0_HIGH 0x6B

// Register addresses
#define LSM6_REG_WHO_AM_I 0x0F
#define LSM6_REG_CTRL1_XL 0x10
#define LSM6_REG_CTRL2_G 0x11
#define LSM6_REG_STATUS_REG 0x1E
#define LSM6_REG_OUTX_L_XL 0x28
#define LSM6_REG_OUTX_H_XL 0x29
#define LSM6_REG_OUTY_L_XL 0x2A
#define LSM6_REG_OUTY_H_XL 0x2B
#define LSM6_REG_OUTZ_L_XL 0x2C
#define LSM6_REG_OUTZ_H_XL 0x2D
#define LSM6_REG_OUTX_L_G 0x22
#define LSM6_REG_OUTX_H_G 0x23
#define LSM6_REG_OUTY_L_G 0x24
#define LSM6_REG_OUTY_H_G 0x25
#define LSM6_REG_OUTZ_L_G 0x26
#define LSM6_REG_OUTZ_H_G 0x27

// Accelerometer full scale ranges
typedef enum {
  LSM6_ACCEL_FS_2G = 0,
  LSM6_ACCEL_FS_16G = 1,
  LSM6_ACCEL_FS_4G = 2,
  LSM6_ACCEL_FS_8G = 3
} lsm6_accel_fs_t;

// Gyroscope full scale ranges
typedef enum {
  LSM6_GYRO_FS_245DPS = 0,
  LSM6_GYRO_FS_500DPS = 1,
  LSM6_GYRO_FS_1000DPS = 2,
  LSM6_GYRO_FS_2000DPS = 3,
  LSM6_GYRO_FS_125DPS = 4
} lsm6_gyro_fs_t;

// Output data rates
typedef enum {
  LSM6_ODR_POWER_DOWN = 0,
  LSM6_ODR_12_5HZ = 1,
  LSM6_ODR_26HZ = 2,
  LSM6_ODR_52HZ = 3,
  LSM6_ODR_104HZ = 4,
  LSM6_ODR_208HZ = 5,
  LSM6_ODR_416HZ = 6,
  LSM6_ODR_833HZ = 7,
  LSM6_ODR_1_66KHZ = 8,
  LSM6_ODR_3_33KHZ = 9,
  LSM6_ODR_6_66KHZ = 10
} lsm6_odr_t;

// Device configuration structure
typedef struct {
  uint8_t i2c_addr;
  lsm6_accel_fs_t accel_fs;
  lsm6_gyro_fs_t gyro_fs;
  lsm6_odr_t accel_odr;
  lsm6_odr_t gyro_odr;
  bool accel_high_performance;
  bool gyro_high_performance;
} lsm6_config_t;

// Sensor structure
typedef struct {
  int16_t accel[3];
  int16_t gyro[3];
  lsm6_config_t config;
} LSM6_HandleTypeDef;

#ifdef __cplusplus
extern "C" {
#endif

  /** Function prototypes */
  bool LSM_Init(LSM6_HandleTypeDef *sens);
  bool LSM_Read_raw(LSM6_HandleTypeDef *sens);
  float LSM_Accel_to_g(int16_t raw, LSM6_HandleTypeDef *sens);
  float LSM_Gyro_to_dps(int16_t raw, LSM6_HandleTypeDef *sens);

#ifdef __cplusplus
}
#endif

#endif  // LSM6DS3TR_H