/*

*/

#ifndef CAN_H_
#define CAN_H_

#include "../includes/ch32v20x.h"

#define RCC_CANEN ((uint32_t)0x02000000)

void can_init(void);

void can_send(void);

#endif 
