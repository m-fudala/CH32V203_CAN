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

void can_send(unsigned int id, unsigned char id_type, unsigned char dlc,
        char data[]) {

    // determine first empty mailbox for transmitting
    unsigned char mailbox;

    if (CAN1->TSTATR & CAN_TSTATR_TME0) {
        mailbox = 0;
    } else if (CAN1->TSTATR & CAN_TSTATR_TME1) {
        mailbox = 1;
    } else {
        mailbox = 2;
    }

    // set ID for data frame
    CAN1->sTxMailBox[mailbox].TXMIR = 0;

    if (!id_type) {
        // standard ID
        CAN1->sTxMailBox[mailbox].TXMIR &= ~CAN_TXMI0R_IDE;
        CAN1->sTxMailBox[mailbox].TXMIR |= id << 21;
    } else {
        // extended ID
        CAN1->sTxMailBox[mailbox].TXMIR |= CAN_TXMI0R_IDE;
        CAN1->sTxMailBox[mailbox].TXMIR |= id << 3;
    }

    CAN1->sTxMailBox[mailbox].TXMIR &= ~CAN_TXMI0R_RTR;

    // don't send timestamp, set DLC
    CAN1->sTxMailBox[mailbox].TXMDTR &= ~CAN_TXMDT0R_TGT;
    CAN1->sTxMailBox[mailbox].TXMDTR |= dlc;

    // write data to registers
    CAN1->sTxMailBox[mailbox].TXMDLR = 0;
    CAN1->sTxMailBox[mailbox].TXMDHR = 0;

    for (unsigned char byte = 0; byte < dlc; ++byte) {
        if (byte < 4) {
            CAN1->sTxMailBox[mailbox].TXMDLR |= data[byte] << (8 * byte);
        } else {
            CAN1->sTxMailBox[mailbox].TXMDHR |= data[byte] << (8 * (byte - 4));
        }
    }

    // request sending
    CAN1->sTxMailBox[mailbox].TXMIR |= 1;

    switch (mailbox) {
        case 0:
            while (!(CAN1->TSTATR & CAN_TSTATR_TXOK0));

            // clear transmision flags
            CAN1->TSTATR |= CAN_TSTATR_TXOK0 | CAN_TSTATR_RQCP0;

            break;

        case 1:
            while (!(CAN1->TSTATR & CAN_TSTATR_TXOK1));

            // clear transmision flags
            CAN1->TSTATR |= CAN_TSTATR_TXOK1 | CAN_TSTATR_RQCP1;

            break;

        case 2:
            while (!(CAN1->TSTATR & CAN_TSTATR_TXOK2));

            // clear transmision flags
            CAN1->TSTATR |= CAN_TSTATR_TXOK2 | CAN_TSTATR_RQCP2;

            break;
    }
}
