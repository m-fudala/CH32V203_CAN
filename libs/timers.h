/*

*/

#ifndef TIMERS_H_
#define TIMERS_H_

#include "../includes/ch32v20x.h"

void pwm_init(void);
void pwm_set_state(unsigned char state);
void pwm_set_duty_cycle(unsigned char duty_cycle);

void tim3_init(void (*handler)(void));
static void (*tim3_handler)(void);
void tim3_set_state(unsigned char state);
void TIM3_IRQHandler(void) __attribute__((interrupt()));

#endif
