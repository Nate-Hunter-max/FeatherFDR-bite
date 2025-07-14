/**
 * @file IICFuncs.h
 * @brief I2C (TWI) communication library for AVR microcontrollers
 * @author Nate Hunter
 * @date 2025-7-14
 * @version v1.0.0
 */

#ifndef IICFUNCS_H
#define IICFUNCS_H

#include <avr/io.h>
#include <util/twi.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Status codes */
#define IIC_SUCCESS     0   ///< Operation completed successfully
#define IIC_ERROR      1   ///< Operation failed (general error)
#define IIC_TIMEOUT    2   ///< Operation timed out

/* Configuration */
#define IIC_TIMEOUT_VALUE  10000   ///< Maximum wait cycles for I2C operations

/**
 * @brief Initialize I2C (TWI) interface
 */
void IIC_Init(void);

/**
 * @brief Write a single byte to a register over I2C
 * @param addr I2C device address
 * @param reg Register address to write to
 * @param data Data byte to write
 * @return Status code (IIC_SUCCESS, IIC_ERROR, or IIC_TIMEOUT)
 */
uint8_t IIC_WriteByte(uint8_t addr, uint8_t reg, uint8_t data);

/**
 * @brief Write multiple bytes to consecutive registers over I2C
 * @param addr I2C device address
 * @param reg Starting register address to write to
 * @param data Pointer to data bytes to write
 * @param num Number of bytes to write
 * @return Status code (IIC_SUCCESS, IIC_ERROR, or IIC_TIMEOUT)
 */
uint8_t IIC_WriteBytes(uint8_t addr, uint8_t reg, const uint8_t* data, uint8_t num);

/**
 * @brief Read a single byte from a register over I2C
 * @param addr I2C device address
 * @param reg Register address to read from
 * @param data Pointer to store the read data byte
 * @return Status code (IIC_SUCCESS, IIC_ERROR, or IIC_TIMEOUT)
 */
uint8_t IIC_ReadByte(uint8_t addr, uint8_t reg, uint8_t* data);

/**
 * @brief Read multiple consecutive registers over I2C
 * @param addr I2C device address
 * @param reg Register address to start reading from
 * @param buffer Pointer to buffer where received data will be stored
 * @param num Number of bytes to read
 * @return Status code (IIC_SUCCESS, IIC_ERROR, or IIC_TIMEOUT)
 */
uint8_t IIC_ReadBytes(uint8_t addr, uint8_t reg, uint8_t* buffer, uint8_t num);

#ifdef __cplusplus
}
#endif

#endif /* IICFUNCS_H */