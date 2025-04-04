#ifndef EPL_H
#define EPL_H

#include "stm32f091xc.h"

#define APB_FREQ 48000000
#define AHB_FREQ 48000000

#define DEBUG

#ifdef DEBUG
  #define LOG( msg... ) printf( msg );
#else
  #define LOG( msg... ) ;
#endif


void EPL_SystemClock_Config(void);


#endif // EPL_CLOCK_H
