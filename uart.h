/**
 * @file uart.h
 * @brief UART library header for ATmega328
 */

#ifndef UART_H
#define UART_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void UART_Init(uint32_t baud);
void UART_Transmit(uint8_t data);
uint8_t UART_Receive();
void UART_TransmitString(const char *str);
void UART_EnablePrintf(void);

#ifdef __cplusplus
}
#endif

#endif /* UART_H */
