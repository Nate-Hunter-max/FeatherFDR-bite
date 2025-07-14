/**
 * @file IICFuncs.c
 * @brief I2C (TWI) communication library implementation
 * @author Nate Hunter
 * @date 2025-7-14
 * @version v1.0.0
 */

#include "twi.h"

/**
 * @brief Internal function to request multiple bytes from a register
 * @param addr I2C device address
 * @param reg Register address to start reading from
 * @param num Number of bytes to request
 * @return Status code (IIC_SUCCESS, IIC_ERROR, or IIC_TIMEOUT)
 */
static uint8_t IIC_Request(uint8_t addr, uint8_t reg, uint8_t num);

/* Public function implementations */

void IIC_Init(void)
{
    /* Set SCL frequency to 100kHz assuming 16MHz clock */
    TWSR = 0x00;         /* Prescaler set to 1 */
    TWBR = 72;           /* SCL frequency = F_CPU / (16 + 2 * TWBR * Prescaler) */
    TWCR = (1 << TWEN);  /* Enable TWI */
}

uint8_t IIC_WriteByte(uint8_t addr, uint8_t reg, uint8_t data)
{
    uint16_t timeout = IIC_TIMEOUT_VALUE;
    
    /* Start condition */
    TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
    while (!(TWCR & (1 << TWINT)) && --timeout);
    if (!timeout) return IIC_TIMEOUT;

    /* Device address with write bit */
    TWDR = (addr << 1);
    TWCR = (1 << TWEN) | (1 << TWINT);
    timeout = IIC_TIMEOUT_VALUE;
    while (!(TWCR & (1 << TWINT)) && --timeout);
    if (!timeout) return IIC_TIMEOUT;

    /* Register address */
    TWDR = reg;
    TWCR = (1 << TWEN) | (1 << TWINT);
    timeout = IIC_TIMEOUT_VALUE;
    while (!(TWCR & (1 << TWINT)) && --timeout);
    if (!timeout) return IIC_TIMEOUT;

    /* Data to write */
    TWDR = data;
    TWCR = (1 << TWEN) | (1 << TWINT);
    timeout = IIC_TIMEOUT_VALUE;
    while (!(TWCR & (1 << TWINT)) && --timeout);
    if (!timeout) return IIC_TIMEOUT;

    /* Stop condition */
    TWCR = (1 << TWSTO) | (1 << TWEN) | (1 << TWINT);
    return IIC_SUCCESS;
}

uint8_t IIC_WriteBytes(uint8_t addr, uint8_t reg, const uint8_t* data, uint8_t num)
{
    uint16_t timeout = IIC_TIMEOUT_VALUE;
    
    /* Start condition */
    TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
    while (!(TWCR & (1 << TWINT)) && --timeout);
    if (!timeout) return IIC_TIMEOUT;

    /* Device address with write bit */
    TWDR = (addr << 1);
    TWCR = (1 << TWEN) | (1 << TWINT);
    timeout = IIC_TIMEOUT_VALUE;
    while (!(TWCR & (1 << TWINT)) && --timeout);
    if (!timeout) return IIC_TIMEOUT;

    /* Starting register address */
    TWDR = reg;
    TWCR = (1 << TWEN) | (1 << TWINT);
    timeout = IIC_TIMEOUT_VALUE;
    while (!(TWCR & (1 << TWINT)) && --timeout);
    if (!timeout) return IIC_TIMEOUT;

    for (uint8_t i = 0; i < num; i++) {
        /* Data byte to write */
        TWDR = data[i];
        TWCR = (1 << TWEN) | (1 << TWINT);
        timeout = IIC_TIMEOUT_VALUE;
        while (!(TWCR & (1 << TWINT)) && --timeout);
        if (!timeout) return IIC_TIMEOUT;
    }

    /* Stop condition */
    TWCR = (1 << TWSTO) | (1 << TWEN) | (1 << TWINT);
    return IIC_SUCCESS;
}

uint8_t IIC_ReadByte(uint8_t addr, uint8_t reg, uint8_t* data)
{
    uint16_t timeout = IIC_TIMEOUT_VALUE;
    
    /* Start condition */
    TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
    while (!(TWCR & (1 << TWINT)) && --timeout);
    if (!timeout) return IIC_TIMEOUT;

    /* Device address with write bit */
    TWDR = (addr << 1);
    TWCR = (1 << TWEN) | (1 << TWINT);
    timeout = IIC_TIMEOUT_VALUE;
    while (!(TWCR & (1 << TWINT)) && --timeout);
    if (!timeout) return IIC_TIMEOUT;

    /* Register address */
    TWDR = reg;
    TWCR = (1 << TWEN) | (1 << TWINT);
    timeout = IIC_TIMEOUT_VALUE;
    while (!(TWCR & (1 << TWINT)) && --timeout);
    if (!timeout) return IIC_TIMEOUT;

    /* Repeated start */
    TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
    timeout = IIC_TIMEOUT_VALUE;
    while (!(TWCR & (1 << TWINT)) && --timeout);
    if (!timeout) return IIC_TIMEOUT;

    /* Device address with read bit */
    TWDR = (addr << 1) | 1;
    TWCR = (1 << TWEN) | (1 << TWINT);
    timeout = IIC_TIMEOUT_VALUE;
    while (!(TWCR & (1 << TWINT)) && --timeout);
    if (!timeout) return IIC_TIMEOUT;

    /* Read without ACK */
    TWCR = (1 << TWEN) | (1 << TWINT);
    timeout = IIC_TIMEOUT_VALUE;
    while (!(TWCR & (1 << TWINT)) && --timeout);
    if (!timeout) return IIC_TIMEOUT;
    
    *data = TWDR;

    /* Stop condition */
    TWCR = (1 << TWSTO) | (1 << TWEN) | (1 << TWINT);
    return IIC_SUCCESS;
}

uint8_t IIC_ReadBytes(uint8_t addr, uint8_t reg, uint8_t* buffer, uint8_t num)
{
    uint16_t timeout;
    
    if (IIC_Request(addr, reg, num)) return IIC_TIMEOUT;
    
    for (uint8_t i = 0; i < num; i++) {
        if (i < num - 1) {
            /* Read with ACK */
            TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWEA);
        } else {
            /* Read without ACK (last byte) */
            TWCR = (1 << TWEN) | (1 << TWINT);
        }
        
        timeout = IIC_TIMEOUT_VALUE;
        while (!(TWCR & (1 << TWINT)) && --timeout);
        if (!timeout) return IIC_TIMEOUT;
        
        buffer[i] = TWDR;
    }
  
    /* Stop condition */
    TWCR = (1 << TWSTO) | (1 << TWEN) | (1 << TWINT);
    return IIC_SUCCESS;
}

/* Private function implementations */

static uint8_t IIC_Request(uint8_t addr, uint8_t reg, uint8_t num)
{
    uint16_t timeout = IIC_TIMEOUT_VALUE;
    
    /* Start condition */
    TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
    while (!(TWCR & (1 << TWINT)) && --timeout);
    if (!timeout) return IIC_TIMEOUT;

    /* Device address with write bit */
    TWDR = (addr << 1);
    TWCR = (1 << TWEN) | (1 << TWINT);
    timeout = IIC_TIMEOUT_VALUE;
    while (!(TWCR & (1 << TWINT)) && --timeout);
    if (!timeout) return IIC_TIMEOUT;

    /* Register address */
    TWDR = reg;
    TWCR = (1 << TWEN) | (1 << TWINT);
    timeout = IIC_TIMEOUT_VALUE;
    while (!(TWCR & (1 << TWINT)) && --timeout);
    if (!timeout) return IIC_TIMEOUT;

    /* Repeated start */
    TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
    timeout = IIC_TIMEOUT_VALUE;
    while (!(TWCR & (1 << TWINT)) && --timeout);
    if (!timeout) return IIC_TIMEOUT;

    /* Device address with read bit */
    TWDR = (addr << 1) | 1;
    TWCR = (1 << TWEN) | (1 << TWINT);
    timeout = IIC_TIMEOUT_VALUE;
    while (!(TWCR & (1 << TWINT)) && --timeout);
    if (!timeout) return IIC_TIMEOUT;

    return IIC_SUCCESS;
}