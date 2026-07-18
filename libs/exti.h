/*

*/

#ifndef EXTI_H_
#define EXTI_H_

#include "../includes/ch32v20x.h"

void exti_init(void (*handler)(void));

static void (*exti_handler)(void);

void EXTI3_IRQHandler(void) __attribute__((interrupt()));

#endif 
