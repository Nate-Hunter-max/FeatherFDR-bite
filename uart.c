/**
 * @file uart.c
 * @brief UART library for ATmega328
 */

#include "uart.h"
#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>

/**
 * @brief Initialize UART with specified baud rate.
 * 
 * @param uart Pointer to UART structure.
 */
void UART_Init(uint32_t baud) {
    uint16_t ubrr = (F_CPU / 16 / baud);
    
    /* Set baud rate */
    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)ubrr;
    
    /* Enable transmitter and receiver */
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    
    /* Set frame format: 8 data bits, 1 stop bit */
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

/**
 * @brief Transmit a single byte via UART.
 * 
 * @param uart Pointer to UART structure.
 * @param data Byte to send.
 */
void UART_Transmit(uint8_t data) {
    /* Wait for empty transmit buffer */
    while (!(UCSR0A & (1 << UDRE0)));
    
    /* Put data into buffer, send the data */
    UDR0 = data;
}

/**
 * @brief Receive a single byte via UART.
 * 
 * @param uart Pointer to UART structure.
 * @return Received byte.
 */
uint8_t UART_Receive() {
    /* Wait for data to be received */
    while (!(UCSR0A & (1 << RXC0)));
    
    /* Get and return received data from buffer */
    return UDR0;
}

/**
 * @brief Transmit a string via UART.
 * 
 * @param uart Pointer to UART structure.
 * @param str Null-terminated string to send.
 */
void UART_TransmitString(const char *str) {
    while (*str) {
        UART_Transmit((uint8_t)*str);
        str++;
    }
}


/**
 * @brief Custom putchar function for printf redirection.
 * 
 * @param data Byte to send.
 * @param stream Unused parameter.
 * @return int Always returns 0.
 */
static int UART_PutChar(char data, FILE *stream) {
    (void)stream; // Unused parameter
    UART_Transmit((uint8_t)data);
    return 0;
}

/**
 * @brief Redirect printf to UART.
 */
void UART_EnablePrintf(void) {
    static FILE uart_stdout = FDEV_SETUP_STREAM(UART_PutChar, NULL, _FDEV_SETUP_WRITE);
    stdout = &uart_stdout;
}
