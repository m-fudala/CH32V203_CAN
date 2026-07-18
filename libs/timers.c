#include "timers.h"

unsigned char tim2_duty_cycle = 0;

void pwm_init() {
    RCC->APB1PCENR |= RCC_TIM2EN;

    RCC->APB2PCENR |= RCC_AFIOEN;
    AFIO->PCFR1 |= AFIO_PCFR1_TIM2_REMAP_0;

    TIM2->CHCTLR1 |= TIM_OC1M_2 | TIM_OC1M_1 | TIM_OC1PE;
    TIM2->CTLR1 |= TIM_ARPE;

    TIM2->CCER |= TIM_CC1E;

    TIM2->SWEVGR |= TIM_UG;

    // SYS_CLK = PB1 clock = 48 MHz
    // PB1 clock / (47 + 1) -> 1 MHz TIM2_CLK
    TIM2->PSC = 47;
    // ARR = 100 -> TIM2 period = 0.0001 -> PWM frequency = 10000 Hz
    // PWM duty cycle is (0; 100)
    TIM2->ATRLR = 99;

    TIM2->CH1CVR = tim2_duty_cycle;

    TIM2->CTLR1 |= TIM_CEN;
}

void pwm_set_state(unsigned char state) {
    TIM2->CNT = 0;

    if (state) {
        pwm_set_duty_cycle(15);
    } else {
        pwm_set_duty_cycle(0);
    }
}

void pwm_set_duty_cycle(unsigned char duty_cycle) {
    tim2_duty_cycle = duty_cycle;

    TIM2->CH1CVR = tim2_duty_cycle;
}

void tim3_init(void (*handler)(void)) {
    tim3_handler = handler;

    RCC->APB1PCENR |= RCC_TIM3EN;

    TIM3->CTLR1 |= TIM_ARPE;

    // SYS_CLK = PB1 clock = 48 MHz
    // PB1 clock / (47 + 1) -> 1 MHz TIM3_CLK
    TIM3->PSC = 4799;
    // ARR = 100 -> TIM3 period = 0.0001 -> PWM frequency = 10000 Hz
    TIM3->CH1CVR = 30000;
    TIM3->ATRLR = 35000;

    TIM3->DMAINTENR |= TIM_CC1IE;
    PFIC->IENR[1] = 1 << 13;    // enable TIM3 in PFIC (45)

    TIM3->CCER |= TIM_CC1E | TIM_CC1P;

    TIM3->SWEVGR |= TIM_UG;

    TIM3->CNT = 0;
}

void tim3_set_state(unsigned char state) {
    TIM3->CNT = 0;

    if (state) {
        TIM3->CTLR1 |= TIM_CEN;
    } else {
        TIM3->CTLR1 &= ~TIM_CEN;
    }
}

void TIM3_IRQHandler(void) {
    tim3_handler();     // invoke function specified in init

    TIM3->CNT = 0;
    TIM3->INTFR &= ~TIM_CC1IF;
}
