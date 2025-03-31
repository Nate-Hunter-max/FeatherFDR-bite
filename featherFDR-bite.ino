#include "bmp280.h"
#include "debugLED.h"
#include "uart.h"
#include "IICFuncs.h"
#include "time.h"

BMP280_HandleTypeDef bmp;

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

  if (BMP280_Init(&bmp) != BMP280_OK) {
    while (1) {
      RGB_SET_COLOR(COLOR_RED);
      _delay_ms(250);
      RGB_SET_COLOR(COLOR_OFF);
      _delay_ms(250);
    };
  };

  for (uint8_t i = 0; i < 100; i++) BMP280_ReadData(&bmp);
  bmp.zeroLvlPress = bmp.pressure;

  uint16_t hue = 0;
  const float hue_step = 1;
  uint8_t r, g, b;

  while (1) {
    static uint32_t ms = TIM_Millis_Get();
    static uint32_t led_ms = TIM_Millis_Get();

    if (TIM_Millis_Get() - ms >= 50) {
      ms = TIM_Millis_Get();
      BMP280_ReadData(&bmp);
      printf(
        "%lu, "
        "%ld.%02ld, "
        "%lu, "
        "%ld;\n",
        ms,
        bmp.temperature / 100,
        abs(bmp.temperature % 100),
        bmp.pressure,
        bmp.altitude);
    }

    if (TIM_Millis_Get() - led_ms >= 2) {
      led_ms = TIM_Millis_Get();
      hsv_to_rgb(hue, 255, 255, &r, &g, &b);
      RGB_SET(r, g, b);
      hue += hue_step;
      if (hue >= 360) hue = 0;
    }
  }
}