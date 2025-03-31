#include "bmp280.h"
#include "IICFuncs.h"
#include <math.h>
#include "debugLED.h"

/** Private function prototypes */
inline void BMP280_ReadCalib(BMP280_HandleTypeDef *bmp);
inline void BMP280_Compensate(BMP280_HandleTypeDef *bmp, int32_t adc_T, int32_t adc_P);
inline void BMP280_ComputeAltitude(BMP280_HandleTypeDef *bmp);

/**
 * @brief Initializes the BMP280 sensor.
 * @param bmp Pointer to BMP280 structure.
 * @return BMP280_Status
 */
BMP280_Status BMP280_Init(BMP280_HandleTypeDef *bmp) {
  // Check ID
  if (IIC_ReadByte(bmp->i2c.adr, BMP280_REG_ID) != bmp->i2c.id)
    return BMP280_ERROR;

  // Read manufacturer calibration data
  BMP280_ReadCalib(bmp);

  // Configure the sensor using stored configuration
  uint8_t ctrl_meas = (bmp->config.mode << 5) | (bmp->config.oversampling << 2) | 0x03;
  uint8_t config = (bmp->config.filter << 2) | 0x00;

  IIC_WriteByte(bmp->i2c.adr, BMP280_REG_CTRL_MEAS, ctrl_meas);
  IIC_WriteByte(bmp->i2c.adr, BMP280_REG_CONFIG, config);

  return BMP280_OK;
}

/**
 * @brief Reads and compensates the pressure and temperature data.
 * @param bmp Pointer to BMP280 structure.
 */
void BMP280_ReadData(BMP280_HandleTypeDef *bmp) {
  uint8_t rx[6];
  IIC_ReadBytes(bmp->i2c.adr, BMP280_REG_PRESS_MSB, rx, 6);
  int32_t adc_P = (int32_t)rx[0] << 12 | (int32_t)rx[1] << 4 | (int32_t)rx[2] >> 4;
  int32_t adc_T = (int32_t)rx[3] << 12 | (int32_t)rx[4] << 4 | (int32_t)rx[5] >> 4;

  BMP280_Compensate(bmp, adc_T, adc_P);
  bmp->altitude = (int32_t)(4433000 * (1.0f - pow((float)bmp->pressure / bmp->zeroLvlPress, 0.1903f)));
}

/**
 * @brief Reads calibration data from BMP280.
 * @param bmp Pointer to BMP280 structure.
 */
void BMP280_ReadCalib(BMP280_HandleTypeDef *bmp) {
  IIC_ReadBytes(bmp->i2c.adr, BMP280_REG_CALIB, (uint8_t *)&(bmp->calib), 24);
}

/**
 * @brief Compensates raw adc data using calibration data.
 * @param bmp Pointer to BMP280 structure.
 * @param adc_P Raw pressure value.
 * @param adc_T Raw temperature value.
 */
void BMP280_Compensate(BMP280_HandleTypeDef *bmp, int32_t adc_T, int32_t adc_P) {
  int32_t var1, var2;

  var1 = ((((adc_T >> 3) - ((int32_t)bmp->calib.T1 << 1))) * ((int32_t)bmp->calib.T2)) >> 11;
  var2 = (((((adc_T >> 4) - ((int32_t)bmp->calib.T1)) * ((adc_T >> 4) - ((int32_t)bmp->calib.T1))) >> 12) * ((int32_t)bmp->calib.T3)) >> 14;
  bmp->temperature = var1 + var2;

  var1 = (((int32_t)bmp->temperature) >> 1) - (int32_t)64000;
  var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * ((int32_t)bmp->calib.P6);
  var2 = var2 + ((var1 * ((int32_t)bmp->calib.P5)) << 1);
  var2 = (var2 >> 2) + (((int32_t)bmp->calib.P4) << 16);
  var1 = (((bmp->calib.P3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3) + ((((int32_t)bmp->calib.P2) * var1) >> 1)) >> 18;
  var1 = ((((32768 + var1)) * ((int32_t)bmp->calib.P1)) >> 15);

  if (var1 == 0) return 0;

  bmp->pressure = (((uint32_t)(((int32_t)1048576) - adc_P) - (var2 >> 12))) * 3125;
  if (bmp->pressure < 0x80000000) bmp->pressure = (bmp->pressure << 1) / ((uint32_t)var1);
  else bmp->pressure = (bmp->pressure / (uint32_t)var1) * 2;
  var1 = (((int32_t)bmp->calib.P9) * ((int32_t)(((bmp->pressure >> 3) * (bmp->pressure >> 3)) >> 13))) >> 12;
  var2 = (((int32_t)(bmp->pressure >> 2)) * ((int32_t)bmp->calib.P8)) >> 13;
  bmp->temperature = (bmp->temperature * 5 + 128) >> 8;
  bmp->pressure = (uint32_t)((int32_t)bmp->pressure + ((var1 + var2 + bmp->calib.P7) >> 4));
}
