/**
 * @file LoRa.h
 * @brief LoRa module driver header file for ATmega328P using custom SPI driver (blocking mode only)
 * @author Nate
 * @date 2025-07-15
 * @version v2.0.0
 */

#ifndef LORA_H
#define LORA_H

#include <stdint.h>
#include "spi_driver.h"

/**
 * @brief LoRa register addresses
 */
typedef enum {
    LORA_REG_FIFO                 = 0x00,
    LORA_REG_OP_MODE              = 0x01,
    LORA_REG_FR_MSB               = 0x06,
    LORA_REG_FR_MID               = 0x07,
    LORA_REG_FR_LSB               = 0x08,
    LORA_REG_PA_CONFIG            = 0x09,
    LORA_REG_LNA                  = 0x0C,
    LORA_REG_FIFO_ADDR_SPI        = 0x0D,
    LORA_REG_FIFO_TX_BASE_ADDR    = 0x0E,
    LORA_REG_FIFO_RX_BASE_ADDR    = 0x0F,
    LORA_REG_FIFO_RX_CURRENT_ADDR = 0x10,
    LORA_REG_IRQ_FLAGS_MASK       = 0x11,
    LORA_REG_IRQ_FLAGS            = 0x12,
    LORA_REG_RX_NB_BYTES          = 0x13,
    LORA_REG_MODEM_CONFIG_1       = 0x1D,
    LORA_REG_MODEM_CONFIG_2       = 0x1E,
    LORA_REG_PREAMBLE_MSB         = 0x20,
    LORA_REG_PREAMBLE_LSB         = 0x21,
    LORA_REG_PAYLOAD_LENGTH       = 0x22,
    LORA_REG_PAYLOAD_MAX_LENGTH   = 0x23,
    LORA_REG_FIFO_RX_BYTE_ADDR    = 0x25,
    LORA_REG_MODEM_CONFIG_3       = 0x26,
    LORA_REG_VERSION              = 0x42
} LoRa_Register_t;

/**
 * @brief IRQ flag bit masks
 */
typedef enum {
    LORA_FLAG_CAD_DETECTED        = 0x01,
    LORA_FLAG_FHSS_CHANGE_CHANNEL = 0x02,
    LORA_FLAG_CAD_DONE            = 0x04,
    LORA_FLAG_TX_DONE             = 0x08,
    LORA_FLAG_VALID_HEADER        = 0x10,
    LORA_FLAG_PAYLOAD_CRC_ERROR   = 0x20,
    LORA_FLAG_RX_DONE             = 0x40,
    LORA_FLAG_RX_TIMEOUT          = 0x80
} LoRa_Flag_t;

/**
 * @brief SPI control bits
 */
typedef enum {
    LORA_SPI_WRITE_BIT = (1 << 7)
} LoRa_SpiBit_t;

/**
 * @brief LoRa configuration structure
 */
typedef struct {
    uint32_t frequency;           /**< Operating frequency in Hz (e.g., 433000000) */
    uint8_t bandwidth;            /**< Bandwidth (0–9) */
    uint8_t spreadingFactor;      /**< Spreading factor (6–12) */
    uint8_t codingRate;           /**< Coding rate (0=4/5 to 3=4/8) */
    uint8_t headerMode;           /**< Header mode (0=explicit, 1=fixed) */
    uint8_t crcEnabled;           /**< CRC enabled (1) or disabled (0) */
    uint8_t lowDataRateOptimize;  /**< Low data rate optimization (0/1) */
    uint8_t preambleLength;       /**< Preamble length (min 4) */
    uint8_t payloadLength;        /**< Payload length (max 255) */
    uint8_t txPower;              /**< Transmission power (0–15) */
    uint8_t txAddr;               /**< FIFO TX base address */
    uint8_t rxAddr;               /**< FIFO RX base address */
} LoRa_Config_t;

/**
 * @brief LoRa handle structure for runtime context
 */
typedef struct {
    SPI_HandleTypeDef *spiHandle;  /**< SPI driver handle */
    volatile uint8_t *nssPort;    /**< Pointer to port register (e.g., &PORTB) */
    uint8_t nssPin;               /**< NSS (CS) pin number (e.g., PB2 → 2) */
    LoRa_Config_t config;         /**< Module configuration */
} LoRa_Handle_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize LoRa module and verify connection
 * @param handle Pointer to LoRa handle structure
 * @return 1 if successful, 0 if chip not found
 */
uint8_t LoRa_Init(LoRa_Handle_t *handle);

/**
 * @brief Apply configuration to LoRa module
 * @param handle Pointer to LoRa handle structure
 * @param config Pointer to configuration
 */
void LoRa_SetConfig(LoRa_Handle_t *handle, LoRa_Config_t *config);

/**
 * @brief Transmit data using LoRa module
 * @param handle Pointer to LoRa handle
 * @param data Pointer to data buffer
 * @param len Number of bytes to transmit
 */
void LoRa_Transmit(LoRa_Handle_t *handle, void *data, uint8_t len);

/**
 * @brief Receive data packet from LoRa
 * @param handle Pointer to LoRa handle
 * @param rxData Pointer to buffer for received data
 * @param len Pointer to variable to store received length
 * @return 1 if valid packet received, 0 otherwise
 */
uint8_t LoRa_Receive(LoRa_Handle_t *handle, void *rxData, uint8_t *len);

/**
 * @brief Enable DIO0 interrupt mapping
 * @param handle Pointer to LoRa handle
 * @param irqMapping Bits 7–6 value to set in RegDioMapping1
 */
void LoRa_EnableDIO0Interrupt(LoRa_Handle_t *handle, uint8_t irqMapping);

/**
 * @brief Disable DIO0 interrupt mapping
 * @param handle Pointer to LoRa handle
 */
void LoRa_DisableDIO0Interrupt(LoRa_Handle_t *handle);

#ifdef __cplusplus
}
#endif

#endif /* LORA_H */
