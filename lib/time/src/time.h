/**
 * @file Time.h
 * @brief Timer1-based millisecond counter and delay functions
 * @author Embedded Systems Engineer
 * @date 2023-11-15
 * @version v1.0.0
 */

#ifndef TIME_H
#define TIME_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize Timer1 millisecond counter
 * @note Must be called before using other TIM functions
 */
void TIM_InitMillis(void);

/**
 * @brief Get current milliseconds count
 * @return Milliseconds since TIM_InitMillis() was called
 */
uint32_t TIM_GetMillis(void);

/**
 * @brief Blocking delay function
 * @param ms Delay duration in milliseconds
 */
void TIM_Delay(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif /* TIME_H */