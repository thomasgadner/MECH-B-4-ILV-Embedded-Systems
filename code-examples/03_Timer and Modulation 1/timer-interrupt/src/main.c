#include "clock_.h"


int main(void){

    EPL_SystemClock_Config();

    RCC->APB2ENR |= RCC_APB2ENR_TIM15EN; 

    // Disable Timer
    // [RM] General Purpose Timer 15/16/17 Page: 542
    TIM15->CR1 &= ~0b1 << 0; // Disable the Timer

    // [RM] General Purpose Timer 15/16/17 Page: 536
    TIM15->PSC = 48-1; // Divide SYSCLK by 48
    // [RM] General Purpose Timer 15/16/17 Page: 536
    TIM15->ARR = 1000-1; // Set the ARR to 1k 


    TIM15->CR1 |= 0b1 << 7; // Enable Auto reload
    TIM15->CR1 |= 0b1 << 2; // Tell Timer to generate an Interrupt at Counter Overflow Event

    // [RM] General Purpose Timer 15/16/17 Page: 529
    TIM15->EGR |= 0b1 << 0; // Re Init the Counter 
    // [RM] General Purpose Timer 15/16/17 Page: 526
    TIM15->DIER |= 0b1 <<0 ; // Enable Update Interrupt

    // [RM] General Purpose Timer 15/16/17 Page: 527
	TIM15->SR &= ~0b01 << 0; // Clear Status Register of the Interrupt

    TIM15->CR1 |= 0b1 << 0;

    DBGMCU->APB2FZ |= DBGMCU_APB2_FZ_DBG_TIM15_STOP;  // Stop the timer when the core is halted for debugging

    // Set IRQ Priority and Enable TIM15_IRQn
    NVIC_SetPriority(TIM15_IRQn, 10);
    NVIC_EnableIRQ(TIM15_IRQn);

    // Enable the IRQ
    __enable_irq();
    for(;;){
        asm("nop");
    }
    return 0;
}

/* For debugging purposes only */
void TIM15_IRQHandler(void) {

    // Add BP here !
    if (TIM15->SR & 0b01){ 
        TIM15->SR &= ~0b01;
    }
}