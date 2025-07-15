/**
 * @file spi_driver.h
 * @brief SPI driver for ATmega328P in master mode (blocking mode)
 * @author Nate Hunter
 * @date 2025-07-14
 * @version v1.0.0
 */

#ifndef SPI_DRIVER_H
#define SPI_DRIVER_H

#include <stdint.h>
#include <avr/io.h>

/**
 * @brief SPI configuration structure
 */
typedef struct {
    uint8_t spiMode;     /**< SPI mode: 0, 1, 2, 3 */
    uint8_t clockDiv;    /**< Clock divider: 4, 16, 64, 128 */
    uint8_t dataOrder;   /**< 0 = MSB first, 1 = LSB first */
} SPI_Config;

/**
 * @brief SPI handle structure
 */
typedef struct {
    SPI_Config config;   /**< SPI configuration */
} SPI_HandleTypeDef;

// Clock divider macros
#define SPI_CLOCK_DIV4    0
#define SPI_CLOCK_DIV16   1
#define SPI_CLOCK_DIV64   2
#define SPI_CLOCK_DIV128  3

// SPI mode macros
#define SPI_MODE0         0  /**< CPOL = 0, CPHA = 0 */
#define SPI_MODE1         1  /**< CPOL = 0, CPHA = 1 */
#define SPI_MODE2         2  /**< CPOL = 1, CPHA = 0 */
#define SPI_MODE3         3  /**< CPOL = 1, CPHA = 1 */

// Data order
#define SPI_MSB_FIRST     0
#define SPI_LSB_FIRST     1

#ifdef __cplusplus
extern "C" {
#endif

void SPI_Init(SPI_HandleTypeDef *hspi);
uint8_t SPI_Transmit(SPI_HandleTypeDef *hspi, uint8_t data);
uint8_t SPI_Receive(SPI_HandleTypeDef *hspi);
uint8_t SPI_TransmitReceive(SPI_HandleTypeDef *hspi, uint8_t data);

#ifdef __cplusplus
}
#endif

#endif // SPI_DRIVER_H
