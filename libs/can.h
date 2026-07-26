/*

*/

#ifndef CAN_H_
#define CAN_H_

#include "../includes/ch32v20x.h"

#define RCC_CANEN ((uint32_t)0x02000000)

enum CanIDType {
    ID_STANDARD,
    ID_EXTENDED
};

void can_init(void);

void can_send(unsigned int id, unsigned char id_type, unsigned char dlc,
        char data[]);

#endif 
