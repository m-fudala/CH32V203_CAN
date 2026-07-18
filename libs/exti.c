#include "exti.h" 

void exti_init(void (*handler)(void)) {
    exti_handler = handler;

    RCC->APB2PCENR |= RCC_IOPBEN | RCC_AFIOEN;   // enable port B clock

    GPIOB->CFGLR = GPIO_CFGLR_CNF3_0;  // PB3: floating input
    AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI3_PB;   // map interrupt to port B

    EXTI->INTENR |= EXTI_INTENR_MR3;     // enable EXTI3 interrupt
    EXTI->FTENR |= EXTI_FTENR_TR3;      // trigger on falling edge
    PFIC->IENR[0] = 1 << 25;    // enable EXTI3 in PFIC (25)
}

void EXTI3_IRQHandler(void) {
    exti_handler();     // invoke function specified in init

    EXTI->INTFR = EXTI_INTF_INTF3;
}