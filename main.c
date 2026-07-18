/*

*/

#include "includes/ch32v20x.h"
#include "libs/can.h"
#include "libs/gpio.h"
#include "libs/exti.h"
#include "libs/uart.h"

void clock_init(void);
void button_handler(void);
void timer_handler(void);
void uart_parse(char *buffer, char buffer_length);

volatile unsigned char led_state = 0;

void button_handler(void) {
    led_state = !led_state;
    set_led_state(led_state);

    can_send();
}

void timer_handler(void) {

}

void uart_parse(char *buffer, char buffer_length) {
    uart_send(buffer, buffer_length);
}

int main(void) {
    clock_init();

    can_init();
    gpio_init(LED_PWM);
    exti_init(button_handler);
    tim3_init(timer_handler);
    uart_init(uart_parse);

    char message[] = "Program start\r\n";
    uart_send(message, 15);

    while(1);
}

void clock_init(void) {
    // configure clock to 48MHz from 16 MHz HSE
    RCC->CTLR |= RCC_HSEON;

    while (!(RCC->CTLR & RCC_HSERDY));   // wait for HSE

    // CFGR0->HPRE = 0  SYSCLK not divided

    RCC->CFGR0 |= RCC_PLLSRC_HSE;    // HSE selected

    RCC->CFGR0 |= RCC_PLLMULL3;     // multiply PLL source *3

    RCC->CTLR |= RCC_PLLON;     // enable PLL

    while (!(RCC->CTLR & RCC_PLLRDY));   // wait for PLL to lock

    RCC->CFGR0 |= RCC_SW_PLL;    // set PLL as clock source

    while (!(RCC->CFGR0 & RCC_SWS_PLL));   // check if PLL is the source

    RCC->CTLR &= ~RCC_HSION;     // disable HSI
}

void HardFault_Handler(void) {
    asm("nop");
}