/**
 * @file Timer1Millis.h
 * @brief Timer1-based millis() replacement for AVR (ATmega328P)
 */

#ifndef TIMER1MILLIS_H
#define TIMER1MILLIS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void TIM_Millis_Init();
uint32_t TIM_Millis_Get();

#ifdef __cplusplus
}
#endif

#endif  // TIMER1MILLIS_H