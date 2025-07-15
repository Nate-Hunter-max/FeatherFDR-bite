/**
 * @file main.cpp
 * @brief Main application entry point for FeatherFDR-bite project.
 *        Initializes sensors, peripherals, and handles main loop logic.
 * @author Nate Hunter
 * @date 2025-07-14
 * @version v1.0.0
 */

#include "bmp280.h"     ///< BMP280 barometric pressure sensor driver
#include "lsm6ds3.h"    ///< LSM6DS3 IMU sensor driver
#include "rgb_led.h"    ///< RGB LED control
#include "uart.h"       ///< UART communication
#include "twi.h"        ///< I2C (TWI) communication
#include "time.h"       ///< Timing utilities
#include "Arduino.h"    ///< Arduino core functions
#include "spi_driver.h"   ///< Custom SPI driver
#include "lora.h"         ///< LoRa radio driver

// BMP280 sensor handle structure
BMP280_HandleTypeDef bmp;

// LSM6DS3 sensor handle structure
LSM6DS3_Handle lsm;

// LoRa handle
static LoRa_Handle_t lora;

// LoRa config
static LoRa_Config_t loraCfg = {
    433000000UL, // frequency
    0x08,        // bandwidth
    7,           // spreadingFactor
    0b001,       // codingRate
    0,           // headerMode
    1,           // crcEnabled
    0,           // lowDataRateOptimize
    6,           // preambleLength
    128,         // payloadLength
    255,         // txAddr
    0,           // rxAddr
    0x01         // txPower
};

static SPI_HandleTypeDef spiHandle = {
    .config = {
        .spiMode = SPI_MODE0,      // SPI mode 0
        .clockDiv = SPI_CLOCK_DIV16, // Clock divider 16
        .dataOrder = SPI_MSB_FIRST  // MSB first
    }
};

/**
 * @brief Helper macro to split float into int and 2-digit fractional part
 * @param x Floating point value
 */
#define PRINT_FLOAT(x) ((int)(x)), (abs((int)((x)*100)) % 100)

/**
 * @brief Main application entry point.
 *        Initializes all peripherals and enters main loop.
 * @return int (never returns)
 */
int main(void) {
  init();                 ///< Initialize Arduino core
  TIM_InitMillis();       ///< Initialize millisecond timer
  IIC_Init();             ///< Initialize I2C interface
  RGB_INIT();             ///< Initialize RGB LED
  UART_Init(115200);      ///< Initialize UART at 115200 baud
  UART_EnablePrintf();    ///< Enable printf over UART

  SPI_Init(&spiHandle);   ///< Initialize SPI hardware
  DDRB |= (1 << PB0);     ///< Set NSS (PB0) as output
  PORTB |= (1 << PB0);    ///< Set NSS high (inactive)

  lora.spiHandle = &spiHandle;         ///< Set SPI handle for LoRa
  lora.nssPort = &PORTB;               ///< NSS port for LoRa
  lora.nssPin = PB0;                   ///< NSS pin for LoRa (PB0)
  lora.config = loraCfg;               ///< Set LoRa configuration
  printf("LoRa Init... %d\n", LoRa_Init(&lora)); ///< Print initialization message

  // BMP280 sensor configuration
  bmp.i2c.adr = 0x76;                         ///< I2C address for BMP280
  bmp.i2c.id = 0x58;                          ///< Device ID for BMP280
  bmp.config.mode = BMP280_MODE_NORMAL;       ///< Normal operation mode
  bmp.config.oversampling = BMP280_OVERSAMPLING_X16; ///< Oversampling x16
  bmp.config.filter = BMP280_FILTER_X16;      ///< Filter coefficient x16

  // LSM6DS3 sensor configuration
  lsm.i2c_addr = 0x6B;                        ///< I2C address for LSM6DS3
  lsm.accelODR = LSM6DS3_ODR_1660HZ;          ///< Accelerometer ODR 1660Hz
  lsm.gyroODR = LSM6DS3_ODR_1660HZ;           ///< Gyroscope ODR 1660Hz

  // Initialize sensors and indicate error with blinking red LED if failed
  if (BMP280_Init(&bmp) != BMP280_OK ||
    !LSM6DS3_Init(&lsm, LSM6DS3_XL_16G, LSM6DS3_GYRO_2000DPS)) {
    while (1) {
      RGB_SET_COLOR(COLOR_RED);   ///< Set LED to red
      _delay_ms(250);
      RGB_SET_COLOR(COLOR_OFF);   ///< Turn off LED
      _delay_ms(250);
    }
  }

  // Discard first 100 BMP280 readings for sensor stabilization
  for (uint8_t i = 0; i < 100; i++) {
    BMP280_ReadData(&bmp);
  }
  bmp.zeroLvlPress = bmp.pressure;    ///< Store baseline pressure

  uint16_t hue = 0;                   ///< Current hue for RGB LED
  const float hueStep = 1;            ///< Hue increment step
  uint8_t r, g, b;                    ///< RGB color values

  // Main loop: read sensors, print data, and animate RGB LED
  while (1) {
    static uint32_t ms = TIM_GetMillis();    ///< Last sensor read time
    static uint32_t ledMs = TIM_GetMillis(); ///< Last LED update time

    // Sensor read and UART print every 50 ms
    if (TIM_GetMillis() - ms >= 50) {
      ms = TIM_GetMillis();

      BMP280_ReadData(&bmp);    ///< Read BMP280 sensor data

      float accel[3], gyro[3];
      LSM6DS3_ReadData(&lsm, accel, gyro); ///< Read IMU data

      printf("T:\t%ld.%02ldC\tP:\t%luPa\tAlt:\t%ldcm\tAx:\t%d.%02d\tAy:\t%d.%02d\tAz:\t%d.%02d\tGx:\t%d.%02d\tGy:\t%d.%02d\tGz:\t%d.%02d\n",
        bmp.temperature / 100, abs(bmp.temperature % 100),
        bmp.pressure, bmp.altitude,
        PRINT_FLOAT(accel[0]), PRINT_FLOAT(accel[1]), PRINT_FLOAT(accel[2]),
        PRINT_FLOAT(gyro[0]), PRINT_FLOAT(gyro[1]), PRINT_FLOAT(gyro[2])
      );
      LoRa_Transmit(&lora, &bmp.pressure, sizeof(bmp.pressure)); ///< Transmit pressure data over LoRa
    }

    // RGB LED color animation update every 2 ms
    if (TIM_GetMillis() - ledMs >= 2) {
      ledMs = TIM_GetMillis();
      hsv_to_rgb(hue, 255, 255, &r, &g, &b); ///< Convert HSV to RGB
      RGB_SET(r, g, b);                       ///< Set RGB LED color
      hue += hueStep;
      if (hue >= 360)
        hue = 0;
    }
  }
}
