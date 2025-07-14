#include "bmp280.h"
#include "lsm6ds3tr.h"
#include "debugLED.h"
#include "uart.h"
#include "IICFuncs.h"
#include "time.h"

BMP280_HandleTypeDef bmp;
LSM6DS3_Handle lsm;

/**
 * @brief Helper macro to split float into int and 2-digit fractional part
 */
#define PRINT_FLOAT(x) ((int)(x)), (abs((int)((x)*100)) % 100)

int main(void) {
  init();
  TIM_Millis_Init();
  IIC_Init();
  RGB_INIT();
  UART_Init(115200);
  UART_EnablePrintf();

  bmp.i2c.adr = 0x76;
  bmp.i2c.id = 0x58;
  bmp.config.mode = BMP280_MODE_NORMAL;
  bmp.config.oversampling = BMP280_OVERSAMPLING_X16;
  bmp.config.filter = BMP280_FILTER_X16;

  lsm.i2c_addr = 0x6B;
  lsm.accelODR = LSM6DS3_ODR_1660HZ;
  lsm.gyroODR = LSM6DS3_ODR_1660HZ;

  if (BMP280_Init(&bmp) != BMP280_OK || !LSM6DS3_Init(&lsm, LSM6DS3_XL_16G, LSM6DS3_GYRO_2000DPS)) {
    while (1) {
      RGB_SET_COLOR(COLOR_RED);
      _delay_ms(250);
      RGB_SET_COLOR(COLOR_OFF);
      _delay_ms(250);
    }
  }

  for (uint8_t i = 0; i < 100; i++) {
    BMP280_ReadData(&bmp);
  }
  bmp.zeroLvlPress = bmp.pressure;

  uint16_t hue = 0;
  const float hueStep = 1;
  uint8_t r, g, b;

  while (1) {
    static uint32_t ms = TIM_Millis_Get();
    static uint32_t ledMs = TIM_Millis_Get();

    if (TIM_Millis_Get() - ms >= 50) {
      ms = TIM_Millis_Get();

      BMP280_ReadData(&bmp);

      float accel[3], gyro[3];
      LSM6DS3_ReadData(&lsm, accel, gyro);

      printf(
        "%lu, "
        "T=%ld.%02ld °C, "
        "P=%lu Pa, "
        "Alt=%ld m, "
        "Ax=%d.%02d g, Ay=%d.%02d g, Az=%d.%02d g, "
        "Gx=%d.%02d dps, Gy=%d.%02d dps, Gz=%d.%02d dps\n",
        ms,
        bmp.temperature / 100,
        abs(bmp.temperature % 100),
        bmp.pressure,
        bmp.altitude,
        PRINT_FLOAT(accel[0]),
        PRINT_FLOAT(accel[1]),
        PRINT_FLOAT(accel[2]),
        PRINT_FLOAT(gyro[0]),
        PRINT_FLOAT(gyro[1]),
        PRINT_FLOAT(gyro[2]));
    }

    if (TIM_Millis_Get() - ledMs >= 2) {
      ledMs = TIM_Millis_Get();
      hsv_to_rgb(hue, 255, 255, &r, &g, &b);
      RGB_SET(r, g, b);
      hue += hueStep;
      if (hue >= 360)
        hue = 0;
    }
  }
}
