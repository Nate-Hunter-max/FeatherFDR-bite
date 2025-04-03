#pragma once
#include <avr/io.h>
#include <util/twi.h>

/**
 * @brief Request multiple bytes from a register over I2C.
 * 
 * @param addr I2C device address.
 * @param reg Register address to start reading from.
 * @param num Number of bytes to request.
 */
void _request(uint8_t addr, uint8_t reg, uint8_t num) {
  TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);  // Start condition
  while (!(TWCR & (1 << TWINT)))
    ;

  TWDR = (addr << 1);  // Device address with write bit
  TWCR = (1 << TWEN) | (1 << TWINT);
  while (!(TWCR & (1 << TWINT)))
    ;

  TWDR = reg;  // Register address
  TWCR = (1 << TWEN) | (1 << TWINT);
  while (!(TWCR & (1 << TWINT)))
    ;

  TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);  // Repeated start
  while (!(TWCR & (1 << TWINT)))
    ;

  TWDR = (addr << 1) | 1;  // Device address with read bit
  TWCR = (1 << TWEN) | (1 << TWINT);
  while (!(TWCR & (1 << TWINT)))
    ;
}

/**
 * @brief Initialize I2C (TWI) interface.
 */
void IIC_Init() {
  // Set SCL frequency to 100kHz assuming 16MHz clock
  TWSR = 0x00;         // Prescaler set to 1
  TWBR = 72;           // SCL frequency = F_CPU / (16 + 2 * TWBR * Prescaler)
  TWCR = (1 << TWEN);  // Enable TWI
}

/**
 * @brief Write a single byte to a register over I2C.
 * 
 * @param addr I2C device address.
 * @param reg Register address to write to.
 * @param data Data byte to write.
 */
void IIC_WriteByte(uint8_t addr, uint8_t reg, uint8_t data) {
  TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);  // Start condition
  while (!(TWCR & (1 << TWINT)))
    ;

  TWDR = (addr << 1);  // Device address with write bit
  TWCR = (1 << TWEN) | (1 << TWINT);
  while (!(TWCR & (1 << TWINT)))
    ;

  TWDR = reg;  // Register address
  TWCR = (1 << TWEN) | (1 << TWINT);
  while (!(TWCR & (1 << TWINT)))
    ;

  TWDR = data;  // Data to write
  TWCR = (1 << TWEN) | (1 << TWINT);
  while (!(TWCR & (1 << TWINT)))
    ;

  TWCR = (1 << TWSTO) | (1 << TWEN) | (1 << TWINT);  // Stop condition
}

/**
 * @brief Write multiple bytes to consecutive registers over I2C.
 * 
 * @param addr I2C device address.
 * @param reg Starting register address to write to.
 * @param data Pointer to data bytes to write.
 * @param num Number of bytes to write.
 */
void IIC_WriteBytes(uint8_t addr, uint8_t reg, const uint8_t* data, uint8_t num) {
  TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);  // Start condition
  while (!(TWCR & (1 << TWINT)))
    ;

  TWDR = (addr << 1);  // Device address with write bit
  TWCR = (1 << TWEN) | (1 << TWINT);
  while (!(TWCR & (1 << TWINT)))
    ;

  TWDR = reg;  // Starting register address
  TWCR = (1 << TWEN) | (1 << TWINT);
  while (!(TWCR & (1 << TWINT)))
    ;

  for (uint8_t i = 0; i < num; i++) {
    TWDR = data[i];  // Data byte to write
    TWCR = (1 << TWEN) | (1 << TWINT);
    while (!(TWCR & (1 << TWINT)))
      ;
  }

  TWCR = (1 << TWSTO) | (1 << TWEN) | (1 << TWINT);  // Stop condition
}

/**
 * @brief Read a single byte from a register over I2C.
 * 
 * @param addr I2C device address.
 * @param reg Register address to read from.
 * @return Read data byte.
 */
uint8_t IIC_ReadByte(uint8_t addr, uint8_t reg) {
  uint8_t res;

  TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);  // Start condition
  while (!(TWCR & (1 << TWINT)))
    ;

  TWDR = (addr << 1);  // Device address with write bit
  TWCR = (1 << TWEN) | (1 << TWINT);
  while (!(TWCR & (1 << TWINT)))
    ;

  TWDR = reg;  // Register address
  TWCR = (1 << TWEN) | (1 << TWINT);
  while (!(TWCR & (1 << TWINT)))
    ;

  TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);  // Repeated start
  while (!(TWCR & (1 << TWINT)))
    ;

  TWDR = (addr << 1) | 1;  // Device address with read bit
  TWCR = (1 << TWEN) | (1 << TWINT);
  while (!(TWCR & (1 << TWINT)))
    ;

  TWCR = (1 << TWEN) | (1 << TWINT);  // Read without ACK
  while (!(TWCR & (1 << TWINT)))
    ;
  res = TWDR;

  TWCR = (1 << TWSTO) | (1 << TWEN) | (1 << TWINT);  // Stop condition
  return res;
}

/**
 * @brief Read multiple consecutive registers over I2C.
 * 
 * @param addr I2C device address.
 * @param reg Register address to start reading from.
 * @param buffer Pointer to buffer where received data will be stored.
 * @param num Number of bytes to read.
 */
void IIC_ReadBytes(uint8_t addr, uint8_t reg, uint8_t* buffer, uint8_t num) {
  _request(addr, reg, num);
  for (uint8_t i = 0; i < num; i++) {
    if (i < num - 1)
      TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWEA);  // Read with ACK
    else
      TWCR = (1 << TWEN) | (1 << TWINT);  // Read without ACK (last byte)
    while (!(TWCR & (1 << TWINT)))
      ;
    buffer[i] = TWDR;
  }
  TWCR = (1 << TWSTO) | (1 << TWEN) | (1 << TWINT);  // Stop condition
}
