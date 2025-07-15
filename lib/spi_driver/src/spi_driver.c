/**
 * @file spi_driver.c
 * @brief SPI driver implementation for ATmega328P using AVR registers in master mode (blocking I/O)
 * @author Nate Hunter
 * @date 2025-07-14
 * @version v1.0.0
 */

#include "spi_driver.h"

/**
 * @brief Initialize SPI in master mode
 * @param hspi Pointer to the SPI handle structure
 */
void SPI_Init(SPI_HandleTypeDef *hspi)
{
    // Set MOSI, SCK, and SS as output
    DDRB |= (1 << PB3) | (1 << PB5) | (1 << PB2); // MOSI, SCK, SS
    DDRB &= ~(1 << PB4); // MISO as input

    // Enable SPI, set as master
    SPCR = (1 << SPE) | (1 << MSTR);

    // Set data order
    if (hspi->config.dataOrder == SPI_LSB_FIRST)
        SPCR |= (1 << DORD);
    else
        SPCR &= ~(1 << DORD);

    // Set SPI mode
    switch (hspi->config.spiMode) {
        case SPI_MODE0:
            SPCR &= ~((1 << CPOL) | (1 << CPHA));
            break;
        case SPI_MODE1:
            SPCR &= ~(1 << CPOL);
            SPCR |= (1 << CPHA);
            break;
        case SPI_MODE2:
            SPCR |= (1 << CPOL);
            SPCR &= ~(1 << CPHA);
            break;
        case SPI_MODE3:
            SPCR |= (1 << CPOL) | (1 << CPHA);
            break;
        default:
            break;
    }

    // Set clock divider
    SPCR &= ~((1 << SPR1) | (1 << SPR0));
    SPSR &= ~(1 << SPI2X);
    switch (hspi->config.clockDiv) {
        case SPI_CLOCK_DIV4:
            break;
        case SPI_CLOCK_DIV16:
            SPCR |= (1 << SPR0);
            break;
        case SPI_CLOCK_DIV64:
            SPCR |= (1 << SPR1);
            break;
        case SPI_CLOCK_DIV128:
            SPCR |= (1 << SPR1) | (1 << SPR0);
            break;
        default:
            break;
    }
}

/**
 * @brief Transmit one byte over SPI
 * @param hspi Pointer to the SPI handle structure
 * @param data Byte to send
 * @return Received byte during transmission
 */
uint8_t SPI_Transmit(SPI_HandleTypeDef *hspi, uint8_t data)
{
    (void)hspi; // Not used for now

    SPDR = data;
    while (!(SPSR & (1 << SPIF))); // Wait for transmission complete

    return SPDR; // Return received byte
}

/**
 * @brief Receive one byte over SPI (sends dummy data)
 * @param hspi Pointer to the SPI handle structure
 * @return Received byte
 */
uint8_t SPI_Receive(SPI_HandleTypeDef *hspi)
{
    return SPI_Transmit(hspi, 0xFF); // Send dummy byte
}

/**
 * @brief Transmit and receive byte simultaneously
 * @param hspi Pointer to the SPI handle structure
 * @param data Byte to send
 * @return Byte received during transmission
 */
uint8_t SPI_TransmitReceive(SPI_HandleTypeDef *hspi, uint8_t data)
{
    return SPI_Transmit(hspi, data);
}
