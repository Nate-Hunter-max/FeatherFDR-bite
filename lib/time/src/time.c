/**
 * @file Time.c
 * @brief Timer1-based millisecond counter and delay functions
 * @author Nate Hunter
 * @date 2025-07-14
 * @version v1.0.0
 */

#include "Time.h"
#include <avr/io.h>
#include <avr/interrupt.h>

/* Private variables */

/**
 * @brief Global milliseconds counter
 * @note Volatile as it's modified in ISR
 */
volatile uint32_t _timer1_millis = 0;

/* Function implementations */

/**
 * @brief Initialize Timer1 for millisecond counter
 * @note Configures Timer1 in CTC mode with 1ms period
 */
void TIM_InitMillis(void)
{
    TCCR1A = 0;  // Clear Timer1 control registers
    TCCR1B = 0;

    // CTC mode, prescaler = 64
    TCCR1B |= (1 << WGM12) | (1 << CS11) | (1 << CS10);

    // 16MHz / 64 / 250 = 1000Hz (1ms)
    OCR1A = 249;
    TIMSK1 |= (1 << OCIE1A);  // Enable compare match interrupt
    sei();                    // Enable global interrupts
}

/**
 * @brief Timer1 compare match interrupt handler
 * @note Increments millisecond counter
 */
ISR(TIMER1_COMPA_vect)
{
    _timer1_millis++;
}

/**
 * @brief Get current milliseconds count
 * @return Current milliseconds since initialization
 * @note Atomic read operation (disables interrupts temporarily)
 */
uint32_t TIM_GetMillis(void)
{
    uint32_t m;
    cli();  // Disable interrupts for atomic read
    m = _timer1_millis;
    sei();
    return m;
}

/**
 * @brief Blocking delay for specified milliseconds
 * @param ms Number of milliseconds to wait
 * @note Uses millis() counter, maintains interrupt capability during wait
 */
void TIM_Delay(uint32_t ms)
{
    uint32_t start = TIM_GetMillis();
    while ((TIM_GetMillis() - start) < ms) {
        // Wait until specified time has elapsed
    }
}