#include "uart.h"

UART uart;

void uart_init(void (*function)(char *, char)) {
    uart.tx_bytes_sent = 0;
    uart.rx_bytes_read = 0;
    idle_function_pointer = function;

    RCC->APB2PCENR |= RCC_IOPAEN;       // enable port A clock
    RCC->APB1PCENR |= RCC_USART2EN;     // enable USART2 clock

    GPIOA->CFGLR |= GPIO_CFGLR_CNF3_0 | GPIO_CFGLR_CNF2_1 | GPIO_CFGLR_MODE2_1;
                            // PA3 (RX): floating input, PA2 (TX): 2 MHz
                            // alternate push-pull output

    USART2->BRR = 12 << 4;  // DIV = 12 -> DIV_Fraction = 0,
                            // DIV_Mantissa = 12 -> 250000 baud @ 48MHz

    // enable RX, TX
    USART2->CTLR1 = USART_CTLR1_UE | USART_CTLR1_TE | USART_CTLR1_RE;

    PFIC->IENR[1] |= 1 << 22;    // enable USART2 global interrupt (54)
    USART2->CTLR1 |= USART_CTLR1_RXNEIE;    // enable RX non-empty interrupt
}

void uart_send(char *bytes, unsigned char bytes_len) {
    uart.tx_bytes_sent = 0;

    uart.tx_bytes_to_send = bytes_len;
    uart.tx_bytes = bytes;

    USART2->CTLR1 |= USART_CTLR1_TXEIE;    // enable TX buffer empty interrupt
}

void USART2_IRQHandler() {
    // RX interrupt
    if (USART2->STATR & USART_STATR_RXNE) {
        uart.rx_buffer[uart.rx_bytes_read] = USART2->DATAR;

        ++uart.rx_bytes_read;

        USART2->STATR |= USART_STATR_RXNE;  // clear RXNE flag
        USART2->CTLR1 |= USART_CTLR1_IDLEIE;    // enable idle line interrupt
    // idle line (RX done) interrupt
    } else if (USART2->STATR & USART_STATR_IDLE) {
        // invoke function on the end of the message
        idle_function_pointer((char *)uart.rx_buffer, uart.rx_bytes_read);

        uart.rx_bytes_read = 0;

        USART2->DATAR;  // read data register to clear the interrupt
        USART2->CTLR1 &= ~USART_CTLR1_IDLEIE;   // disable idle line interrupt
    // TX interrupt
    } else if (USART2->STATR & USART_STATR_TXE) {
        // put a byte into data register
        USART2->DATAR = *(uart.tx_bytes + uart.tx_bytes_sent);

        ++uart.tx_bytes_sent;

        if (uart.tx_bytes_sent == uart.tx_bytes_to_send) {  
            USART2->CTLR1 &= ~USART_CTLR1_TXEIE;    // disable TX interrupt
                                                    // when all bytes are sent
        }
    }
}