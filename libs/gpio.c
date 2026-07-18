#include "gpio.h"

unsigned char led_type;

void gpio_init(LEDFunctions type) {
    RCC->APB2PCENR |= RCC_IOPAEN;   // enable port A clock

    led_type = type;

    switch (led_type) {
        case LED_STANDARD: {
            GPIOA->CFGHR = GPIO_CFGHR_MODE15_1;     // PA15: 2 MHz, push-pull
                                                    // output

            break;
        }

        case LED_PWM: {
            GPIOA->CFGHR = GPIO_CFGHR_CNF15_1 | GPIO_CFGHR_MODE15_1;
                    // PA15: 2 MHz, push-pull alternate output

            pwm_init();

            break;
        }
    }
}

void set_led_state(unsigned char state) {
    switch (led_type) {
        case LED_STANDARD: {
            if (state) {
                GPIOA->OUTDR |= GPIO_OUTDR_ODR15;
            } else {
                GPIOA->OUTDR &= ~GPIO_OUTDR_ODR15;
            }

            break;
        }

        case LED_PWM: {
            pwm_set_state(state);

            break;
        }
    }
}

void set_led_duty_cycle(unsigned char duty_cycle) {
    if (led_type == LED_PWM) {
        pwm_set_duty_cycle(duty_cycle);
    }
}