/*

*/

#ifndef GPIO_H_
#define GPIO_H_

#include "../includes/ch32v20x.h"
#include "timers.h"

typedef enum {
    LED_STANDARD,
    LED_PWM
} LEDFunctions;

void gpio_init(LEDFunctions type);
void set_led_state(unsigned char state);
void set_led_duty_cycle(unsigned char duty_cycle);

#endif
