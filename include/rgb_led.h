/**
 * @file rgb_led.h
 * @brief RGB LED PWM control for Common Anode (AVR/Arduino)
 * @details Uses hardware PWM (Timer0 for D5/D6, Timer2 for D3)
 */

#ifndef RGB_LED_H
#define RGB_LED_H

#include <avr/io.h>
#include <util/delay.h>

// PWM pins (ATmega328P)
#define RED_PIN 3    // PD3 (OC2B, Timer2)
#define GREEN_PIN 5  // PD5 (OC0B, Timer0)
#define BLUE_PIN 6   // PD6 (OC0A, Timer0)

// Inverted PWM logic (Common Anode: 0=ON, 255=OFF)
#define COLOR_RED 0, 255, 255
#define COLOR_GREEN 255, 0, 255
#define COLOR_BLUE 255, 255, 0
#define COLOR_WHITE 0, 0, 0
#define COLOR_YELLOW 0, 0, 255
#define COLOR_CYAN 255, 0, 0
#define COLOR_PURPLE 0, 255, 0
#define COLOR_OFF 255, 255, 255

/**
 * @brief Initialize PWM timers for RGB control
 */
#define RGB_INIT() \
  do { \
    /* Set pins as outputs */ \
    DDRD |= (1 << RED_PIN) | (1 << GREEN_PIN) | (1 << BLUE_PIN); \
\
    /* Timer0 (Fast PWM, 8-bit) for Green/Blue */ \
    TCCR0A = (1 << COM0A1) | (1 << COM0B1) | (1 << WGM01) | (1 << WGM00); \
    TCCR0B = (1 << CS00); /* No prescaler (~62.5kHz PWM) */ \
\
    /* Timer2 (Fast PWM, 8-bit) for Red */ \
    TCCR2A = (1 << COM2B1) | (1 << WGM21) | (1 << WGM20); \
    TCCR2B = (1 << CS20); /* No prescaler (~31.4kHz PWM) */ \
\
    /* Start with all channels OFF */ \
    OCR0A = 255; /* Blue */ \
    OCR0B = 255; /* Green */ \
    OCR2B = 255; /* Red */ \
  } while (0)

/**
 * @brief Set RGB PWM values (0=ON, 255=OFF)
 * @param r Red value (0-255)
 * @param g Green value (0-255)
 * @param b Blue value (0-255)
 */
#define RGB_SET(r, g, b) \
  do { \
    OCR2B = (r); /* Timer2 for Red */ \
    OCR0B = (g); /* Timer0 for Green */ \
    OCR0A = (b); /* Timer0 for Blue */ \
  } while (0)

/**
 * @brief Set predefined color
 * @param color Triple of inverted PWM values
 */
#define RGB_SET_COLOR(color) RGB_SET(color)

/**
 * @brief Convert HSV to RGB using integer math
 * @param h Hue (0-360)
 * @param s Saturation (0-255)
 * @param v Value (0-255)
 * @param[out] r Red component (0-255)
 * @param[out] g Green component (0-255)
 * @param[out] b Blue component (0-255)
 */
void hsv_to_rgb(uint16_t h, uint8_t s, uint8_t v, uint8_t *r, uint8_t *g, uint8_t *b) {
  uint8_t region, remainder, p, q, t;

  region = h / 60;                  // 0 to 5
  remainder = (h % 60) * 255 / 60;  // Scale to 0-255

  p = (v * (255 - s)) / 255;
  q = (v * (255 - (s * remainder) / 255)) / 255;
  t = (v * (255 - (s * (255 - remainder)) / 255)) / 255;

  switch (region) {
    case 0:
      *r = v;
      *g = t;
      *b = p;
      break;
    case 1:
      *r = q;
      *g = v;
      *b = p;
      break;
    case 2:
      *r = p;
      *g = v;
      *b = t;
      break;
    case 3:
      *r = p;
      *g = q;
      *b = v;
      break;
    case 4:
      *r = t;
      *g = p;
      *b = v;
      break;
    default:
      *r = v;
      *g = p;
      *b = q;
      break;
  }
}

#endif  // RGB_LED_H