/**
 * @file lsm6ds3tr.h
 * @brief LSM6DS3TR-C 3D accelerometer and gyroscope I2C driver for Atmega328
 * @author Nate Hunter
 * @date 2025-07-14
 * @version v2.1.0
 */

#ifndef LSM6DS3_H
#define LSM6DS3_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup LSM6DS3 Constants and Registers
 * @{
 */

/// WHO_AM_I expected values
#define LSM6DS3TR_WHO_AM_I     0x69
#define LSM6DS3_WHO_AM_I       0x6A

/// LSM6DS3 Register Map
#define LSM6DS3_REG_WHO_AM_I   0x0F  /**< Device identification register */
#define LSM6DS3_REG_CTRL1_XL   0x10  /**< Accelerometer control register */
#define LSM6DS3_REG_CTRL2_G    0x11  /**< Gyroscope control register */
#define LSM6DS3_REG_CTRL3_C    0x12  /**< Common settings register */
#define LSM6DS3_REG_OUTX_L_G   0x22  /**< Gyroscope output register start */
#define LSM6DS3_REG_OUTX_L_XL  0x28  /**< Accelerometer output register start */

/** @} */

/** @brief Accelerometer full-scale range (g) */
typedef enum {
    LSM6DS3_XL_2G  = 0,
    LSM6DS3_XL_4G  = 2,
    LSM6DS3_XL_8G  = 3,
    LSM6DS3_XL_16G = 1
} LSM6DS3_AccelFS;

/** @brief Gyroscope full-scale range (dps) */
typedef enum {
    LSM6DS3_GYRO_125DPS  = 1,
    LSM6DS3_GYRO_250DPS  = 0,
    LSM6DS3_GYRO_500DPS  = 2,
    LSM6DS3_GYRO_1000DPS = 4,
    LSM6DS3_GYRO_2000DPS = 6
} LSM6DS3_GyroFS;

/** @brief Output data rate settings for accelerometer and gyroscope */
typedef enum {
    LSM6DS3_ODR_OFF     = 0x00,
    LSM6DS3_ODR_12HZ5   = 0x01,
    LSM6DS3_ODR_26HZ    = 0x02,
    LSM6DS3_ODR_52HZ    = 0x03,
    LSM6DS3_ODR_104HZ   = 0x04,
    LSM6DS3_ODR_208HZ   = 0x05,
    LSM6DS3_ODR_416HZ   = 0x06,
    LSM6DS3_ODR_833HZ   = 0x07,
    LSM6DS3_ODR_1660HZ  = 0x08,
    LSM6DS3_ODR_3330HZ  = 0x09,
    LSM6DS3_ODR_6660HZ  = 0x0A
} LSM6DS3_ODR;

/**
 * @brief LSM6DS3 device handle
 */
typedef struct {
    uint8_t i2c_addr;         /**< I2C device address (7-bit) */
    uint32_t timeout;         /**< Communication timeout (not used with IIC_ API) */

    float accelScale;         /**< Accelerometer scale factor (g/LSB) */
    float gyroScale;          /**< Gyroscope scale factor (dps/LSB) */

    LSM6DS3_ODR accelODR;     /**< Accelerometer output data rate */
    LSM6DS3_ODR gyroODR;      /**< Gyroscope output data rate */
} LSM6DS3_Handle;

/**
 * @brief Initialize the LSM6DS3 device
 * 
 * @param dev Pointer to the device handle
 * @param accelFS Accelerometer full-scale setting
 * @param gyroFS Gyroscope full-scale setting
 * @return 1 on success, 0 on failure
 */
uint8_t LSM6DS3_Init(LSM6DS3_Handle *dev,
                     LSM6DS3_AccelFS accelFS,
                     LSM6DS3_GyroFS gyroFS);

/**
 * @brief Read acceleration and gyroscope data from LSM6DS3
 * 
 * @param dev Pointer to the device handle
 * @param accel Output array for 3-axis acceleration (X, Y, Z) in g
 * @param gyro Output array for 3-axis gyroscope (X, Y, Z) in dps
 * @return 1 on success, 0 on failure
 */
uint8_t LSM6DS3_ReadData(LSM6DS3_Handle *dev, float accel[3], float gyro[3]);

#ifdef __cplusplus
}
#endif

#endif // LSM6DS3_H
