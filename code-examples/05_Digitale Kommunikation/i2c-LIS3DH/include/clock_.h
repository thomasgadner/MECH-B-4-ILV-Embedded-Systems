#ifndef CLOCK__H
#define CLOCK__H

#include "stm32f091xc.h"

#define APB_FREQ 48000000
#define AHB_FREQ 48000000

#define BAUDRATE 9600


void SystemClock_Config(void);
void Init_Debug_UART();
void delay(uint32_t time);
#endif 
