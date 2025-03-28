/**
 * @file Time.c
 * @brief Implementation of Timer1-based millis().
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "time.h"

volatile uint32_t _timer1_millis = 0;  // Milliseconds counter

void TIM_Millis_Init() {
  TCCR1A = 0;  // Clear Timer1 control registers
  TCCR1B = 0;

  // CTC mode, prescaler = 64
  TCCR1B |= (1 << WGM12) | (1 << CS11) | (1 << CS10);

  // 16MHz / 64 / 250 = 1000Hz (1ms)
  OCR1A = 249;
  TIMSK1 |= (1 << OCIE1A);  // Enable compare match interrupt
  sei();                    // Enable global interrupts
}

ISR(TIMER1_COMPA_vect) {
  _timer1_millis++;
}

uint32_t TIM_Millis_Get() {
  uint32_t m;
  cli();  // Disable interrupts for atomic read
  m = _timer1_millis;
  sei();
  return m;
}