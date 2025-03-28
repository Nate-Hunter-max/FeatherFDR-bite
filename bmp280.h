#ifndef BMP280_H
#define BMP280_H

#include <stdint.h>

/**
 * @brief BMP280 register addresses.
 */
#define BMP280_REG_ID 0xD0
#define BMP280_REG_RESET 0xE0
#define BMP280_REG_CTRL_MEAS 0xF4
#define BMP280_REG_CONFIG 0xF5
#define BMP280_REG_PRESS_MSB 0xF7
#define BMP280_REG_PRESS_LSB 0xF8
#define BMP280_REG_PRESS_XLSB 0xF9
#define BMP280_REG_TEMP_MSB 0xFA
#define BMP280_REG_TEMP_LSB 0xFB
#define BMP280_REG_TEMP_XLSB 0xFC
#define BMP280_REG_CALIB 0x88

// Adr = 0x76, ID = 0x56
#define BMP280_DEFULT_BUS ((BMP280_Bus)(0x76 | (0x58 << 8)))

/**
 * @brief BMP280 operation modes.
 */
typedef enum {
  BMP280_MODE_SLEEP = 0x00,
  BMP280_MODE_FORCED = 0x01,
  BMP280_MODE_NORMAL = 0x03
} BMP280_Mode;

/**
 * @brief BMP280 oversampling settings.
 */
typedef enum {
  BMP280_OVERSAMPLING_SKIP = 0x00,
  BMP280_OVERSAMPLING_X1 = 0x01,
  BMP280_OVERSAMPLING_X2 = 0x02,
  BMP280_OVERSAMPLING_X4 = 0x03,
  BMP280_OVERSAMPLING_X8 = 0x04,
  BMP280_OVERSAMPLING_X16 = 0x05
} BMP280_Oversampling;

/**
 * @brief BMP280 IIR filter settings.
 */
typedef enum {
  BMP280_FILTER_OFF = 0x00,
  BMP280_FILTER_X2 = 0x01,
  BMP280_FILTER_X4 = 0x02,
  BMP280_FILTER_X8 = 0x03,
  BMP280_FILTER_X16 = 0x04
} BMP280_Filter;

/**
 * @brief BMP280 status.
 */
typedef enum {
  BMP280_OK = 0,
  BMP280_ERROR
} BMP280_Status;

/**
 * @brief BMP280 I2C settings.
 */
typedef struct {
  uint8_t adr;
  uint8_t id;
} BMP280_Bus;

/**
 * @brief BMP280 calibration data structure.
 */
typedef struct {
  uint16_t T1;
  int16_t T2, T3;
  uint16_t P1;
  int16_t P2, P3, P4, P5, P6, P7, P8, P9;
} BMP280_CalibData;

/**
 * @brief BMP280 configuration structure.
 */
typedef struct {
  BMP280_Mode mode;
  BMP280_Oversampling oversampling;
  BMP280_Filter filter;
} BMP280_Config;

/**
 * @brief BMP280 device handle structure.
 */
typedef struct {
  BMP280_Bus i2c;         /**< I2C settings */
  BMP280_Config config;   /**< Sensor configuration */
  BMP280_CalibData calib; /**< Calibration data */
  int32_t temperature;    /**< Last measured temperature (°C * 100) */
  uint32_t pressure;      /**< Last measured pressure (Pa) */
  uint32_t zeroLvlPress;  /**< Initial pressure for altitude calculation (Pa) */
  int32_t altitude;       /**< Last measured altitude (cm) */
} BMP280_HandleTypeDef;

#ifdef __cplusplus
extern "C" {
#endif

/** Function prototypes */
BMP280_Status BMP280_Init(BMP280_HandleTypeDef *bmp);
void BMP280_ReadData(BMP280_HandleTypeDef *bmp);

#ifdef __cplusplus
}
#endif

#endif /* BMP280_H */
