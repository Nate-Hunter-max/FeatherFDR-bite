/**
 * @file lora.c
 * @brief LoRa module driver implementation using custom SPI driver for ATmega328P (blocking I/O mode)
 * @author Nate Hunter
 * @date 2025-07-15
 * @version v2.0.0
 */

#include "lora.h"
#include "spi_driver.h"
#include <string.h>

/* Static buffer for SPI transactions */
static uint8_t txBuffer[1];

/* Private function prototypes */
static inline void LoRa_WriteReg(LoRa_Handle_t *handle, LoRa_Register_t reg, uint8_t *data, uint8_t count);
static inline void LoRa_WriteRegByte(LoRa_Handle_t *handle, LoRa_Register_t reg, uint8_t data);
static inline uint8_t LoRa_ReadRegByte(LoRa_Handle_t *handle, LoRa_Register_t reg);
static inline void LoRa_ReadReg(LoRa_Handle_t *handle, LoRa_Register_t reg, uint8_t *data, uint8_t count);

/**
 * @brief Initialize the LoRa module
 * @param handle Pointer to LoRa handle structure
 * @return 1 if successful, 0 if failed
 */
uint8_t LoRa_Init(LoRa_Handle_t *handle)
{
    uint8_t id;

    txBuffer[0] = LORA_REG_VERSION & ~LORA_SPI_WRITE_BIT;
    *(handle->nssPort) &= ~(1 << handle->nssPin);
    SPI_Transmit(handle->spiHandle, txBuffer[0]);
    id = SPI_Transmit(handle->spiHandle, 0xFF);
    *(handle->nssPort) |= (1 << handle->nssPin);
    printf("LoRa ID: 0x%02X\n", id);

    if (id != 0x12) {
        return 0;
    }

    LoRa_WriteRegByte(handle, LORA_REG_OP_MODE, 0x00);
    LoRa_SetConfig(handle, &handle->config);

    return 1;
}

/**
 * @brief Configure LoRa parameters
 * @param handle Pointer to LoRa handle
 * @param config Pointer to configuration
 */
void LoRa_SetConfig(LoRa_Handle_t *handle, LoRa_Config_t *config)
{
    LoRa_WriteRegByte(handle, LORA_REG_OP_MODE, 0x80); // Standby
    handle->config = *config;

    uint32_t freq = config->frequency * 16384;
    uint8_t fr[3] = { (uint8_t)(freq >> 16), (uint8_t)(freq >> 8), (uint8_t)(freq) };

    uint16_t preamble = config->preambleLength;
    uint8_t pr[2] = { (uint8_t)(preamble >> 8), (uint8_t)(preamble) };

    LoRa_WriteReg(handle, LORA_REG_FR_MSB, fr, 3);
    LoRa_WriteRegByte(handle, LORA_REG_MODEM_CONFIG_1,
                      (config->bandwidth << 4) | (config->codingRate << 1) | config->headerMode);
    LoRa_WriteRegByte(handle, LORA_REG_MODEM_CONFIG_2,
                      (config->spreadingFactor << 4) | (config->crcEnabled << 2));
    LoRa_WriteRegByte(handle, LORA_REG_MODEM_CONFIG_3,
                      (config->lowDataRateOptimize << 3));
    LoRa_WriteReg(handle, LORA_REG_PREAMBLE_MSB, pr, 2);
    LoRa_WriteRegByte(handle, LORA_REG_PAYLOAD_LENGTH, config->payloadLength);
    LoRa_WriteRegByte(handle, LORA_REG_PAYLOAD_MAX_LENGTH, config->payloadLength);
    LoRa_WriteRegByte(handle, LORA_REG_LNA, (1 << 5) | 0x03);
    LoRa_WriteRegByte(handle, LORA_REG_PA_CONFIG,
                      (1 << 7) | (0x07 << 4) | config->txPower);
    LoRa_WriteRegByte(handle, LORA_REG_FIFO_TX_BASE_ADDR, config->txAddr);
    LoRa_WriteRegByte(handle, LORA_REG_FIFO_RX_BASE_ADDR, config->rxAddr);
    LoRa_WriteRegByte(handle, LORA_REG_OP_MODE, 0x89); // LoRa mode, standby
    LoRa_WriteRegByte(handle, LORA_REG_OP_MODE, 0x05); // Rx continuous
}

/**
 * @brief Transmit a packet via LoRa
 * @param handle Pointer to LoRa handle
 * @param data Pointer to TX buffer
 * @param len Number of bytes to transmit
 */
void LoRa_Transmit(LoRa_Handle_t *handle, void *data, uint8_t len)
{
    LoRa_WriteRegByte(handle, LORA_REG_OP_MODE, 0x01); // standby
    LoRa_WriteRegByte(handle, LORA_REG_PAYLOAD_LENGTH, len);
    LoRa_WriteRegByte(handle, LORA_REG_FIFO_ADDR_SPI, handle->config.txAddr);

    *(handle->nssPort) &= ~(1 << handle->nssPin);
    SPI_Transmit(handle->spiHandle, LORA_REG_FIFO | LORA_SPI_WRITE_BIT);

    for (uint8_t i = 0; i < len; ++i) {
        SPI_Transmit(handle->spiHandle, ((uint8_t*)data)[i]);
    }

    *(handle->nssPort) |= (1 << handle->nssPin);

    LoRa_WriteRegByte(handle, LORA_REG_OP_MODE, 0x83); // Tx mode

    while (!(LoRa_ReadRegByte(handle, LORA_REG_IRQ_FLAGS) & LORA_FLAG_TX_DONE));

    LoRa_WriteRegByte(handle, LORA_REG_IRQ_FLAGS, LORA_FLAG_TX_DONE);
    LoRa_WriteRegByte(handle, LORA_REG_OP_MODE, 0x05); // back to Rx
}

/**
 * @brief Receive data if packet is available
 * @param handle Pointer to LoRa handle
 * @param rxData Pointer to RX buffer
 * @param len Pointer to store length of received data
 * @return 1 if valid packet received, 0 otherwise
 */
uint8_t LoRa_Receive(LoRa_Handle_t *handle, void *rxData, uint8_t *len)
{
    uint8_t irqFlags = LoRa_ReadRegByte(handle, LORA_REG_IRQ_FLAGS);

    if (!(irqFlags & LORA_FLAG_RX_DONE)) {
        return 0;
    }

    if (handle->config.crcEnabled &&
        (irqFlags & LORA_FLAG_PAYLOAD_CRC_ERROR)) {
        LoRa_WriteRegByte(handle, LORA_REG_IRQ_FLAGS, LORA_FLAG_PAYLOAD_CRC_ERROR);
        return 0;
    }

    uint8_t currentAddr = LoRa_ReadRegByte(handle, LORA_REG_FIFO_RX_CURRENT_ADDR);
    LoRa_WriteRegByte(handle, LORA_REG_FIFO_ADDR_SPI, currentAddr);
    *len = LoRa_ReadRegByte(handle, LORA_REG_RX_NB_BYTES);

    LoRa_ReadReg(handle, LORA_REG_FIFO, rxData, *len);

    LoRa_WriteRegByte(handle, LORA_REG_IRQ_FLAGS, LORA_FLAG_RX_DONE);

    return 1;
}

/**
 * @brief Set IRQ mapping for DIO0 pin
 * @param handle Pointer to LoRa handle
 * @param irqMapping Mapping to set in bits 7-6 of RegDioMapping1
 */
void LoRa_EnableDIO0Interrupt(LoRa_Handle_t *handle, uint8_t irqMapping)
{
    uint8_t reg = LoRa_ReadRegByte(handle, 0x40);
    reg &= ~0xC0;
    reg |= (irqMapping << 6);
    LoRa_WriteRegByte(handle, 0x40, reg);
}

/**
 * @brief Disable DIO0 IRQ mapping
 * @param handle Pointer to LoRa handle
 */
void LoRa_DisableDIO0Interrupt(LoRa_Handle_t *handle)
{
    uint8_t reg = LoRa_ReadRegByte(handle, 0x40);
    reg &= ~0xC0;
    LoRa_WriteRegByte(handle, 0x40, reg);
}

/* Private helpers */

/**
 * @brief Write multiple bytes to register
 */
static inline void LoRa_WriteReg(LoRa_Handle_t *handle, LoRa_Register_t reg, uint8_t *data, uint8_t count)
{
    *(handle->nssPort) &= ~(1 << handle->nssPin);
    SPI_Transmit(handle->spiHandle, reg | LORA_SPI_WRITE_BIT);
    for (uint8_t i = 0; i < count; ++i) {
        SPI_Transmit(handle->spiHandle, data[i]);
    }
    *(handle->nssPort) |= (1 << handle->nssPin);
}

/**
 * @brief Write single byte to register
 */
static inline void LoRa_WriteRegByte(LoRa_Handle_t *handle, LoRa_Register_t reg, uint8_t data)
{
    LoRa_WriteReg(handle, reg, &data, 1);
}

/**
 * @brief Read single byte from register
 */
static inline uint8_t LoRa_ReadRegByte(LoRa_Handle_t *handle, LoRa_Register_t reg)
{
    uint8_t result = 0;
    *(handle->nssPort) &= ~(1 << handle->nssPin);
    SPI_Transmit(handle->spiHandle, reg & ~LORA_SPI_WRITE_BIT);
    result = SPI_Transmit(handle->spiHandle, 0xFF);
    *(handle->nssPort) |= (1 << handle->nssPin);
    return result;
}

/**
 * @brief Read multiple bytes from register
 */
static inline void LoRa_ReadReg(LoRa_Handle_t *handle, LoRa_Register_t reg, uint8_t *data, uint8_t count)
{
    *(handle->nssPort) &= ~(1 << handle->nssPin);
    SPI_Transmit(handle->spiHandle, reg & ~LORA_SPI_WRITE_BIT);
    for (uint8_t i = 0; i < count; ++i) {
        data[i] = SPI_Transmit(handle->spiHandle, 0xFF);
    }
    *(handle->nssPort) |= (1 << handle->nssPin);
}
