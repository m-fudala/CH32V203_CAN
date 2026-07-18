#include "can.h" 

void can_init(void) {
    // GPIO settings
    RCC->APB2PCENR |= RCC_IOPAEN;   // enable port A clock
    // TX - PA12 -> push-pull alternate output
    // RX - PA11 -> pull-up input
    // overwriting port A, this init has to be done first
    GPIOA->CFGHR = GPIO_CFGHR_CNF12_1 | GPIO_CFGHR_CNF11_1 |
            GPIO_CFGHR_MODE12_1 | GPIO_CFGHR_MODE12_0;
    GPIOA->OUTDR = GPIO_OUTDR_ODR11;

    RCC->APB1PCENR |= RCC_CANEN;     // enable CAN1 clock

    // exit sleep mode and enter initialization mode
    CAN1->CTLR &= ~CAN_CTLR_SLEEP;
    CAN1->CTLR |= CAN_CTLR_INRQ;
    while (!(CAN1->STATR & CAN_STATR_INAK));

    // set 500 kbps speed with 48MHz clock: TS1 = 13, TS2 = 2, BRP = 6
    CAN1->BTIMR = (2 - 1) << 20 | (13 - 1) << 16 | (6 - 1);

    CAN1->CTLR &= ~1 << 16;
    CAN1->STATR |= CAN_STATR_WKUI;

    // exit initialization mode
    CAN1->CTLR &= ~CAN_CTLR_INRQ;
    while (CAN1->STATR & CAN_STATR_INAK);
}

void can_send(void) {
    // standard ID 0x15 data frame
    CAN1->sTxMailBox[0].TXMIR = 0x15 << 21;
    CAN1->sTxMailBox[0].TXMIR &= ~(CAN_TXMI0R_IDE | CAN_TXMI0R_RTR);

    // don't send timestamp, DLC of 8
    CAN1->sTxMailBox[0].TXMDTR &= ~CAN_TXMDT0R_TGT;
    CAN1->sTxMailBox[0].TXMDTR |= 8;

    CAN1->sTxMailBox[0].TXMDHR = 0x88 << 24 | 0x77 << 16 |
            0x66 << 8 | 0x55;
    CAN1->sTxMailBox[0].TXMDLR = 0x44 << 24 | 0x33 << 16 |
            0x22 << 8 | 0x11;

    // request sending
    CAN1->sTxMailBox[0].TXMIR |= CAN_TXMI0R_TXRQ;
    while (!(CAN1->TSTATR & CAN_TSTATR_TXOK0));

    // clear transmision flags
    CAN1->TSTATR |= CAN_TSTATR_TXOK0 | CAN_TSTATR_RQCP0;
}
