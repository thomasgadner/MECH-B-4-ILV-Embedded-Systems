#include "epl.h"
#define ADC_TIMEOUT 100000


void init_tim15(void){
    // Enable the TIM2 clock
    RCC->APB2ENR |= RCC_APB2ENR_TIM15EN; 
    // Disable the timer
    TIM15->CR1 &= ~TIM_CR1_CEN;
    
    TIM15->PSC = 48-1;
    TIM15->ARR = 1000-1;
    TIM15->CR1 |= TIM_CR1_ARPE;
    TIM15->CR1 |= TIM_CR1_URS;
    TIM15->EGR |= TIM_EGR_UG;

    // Output a TRGO event on update for ADC
    TIM15->CR2 |= 0x2 << TIM_CR2_MMS_Pos;

    //TIM15->DIER |= TIM_DIER_UIE;
	TIM15->SR &= ~TIM_SR_UIF;    // Clear the interrupt

    TIM15->EGR |= TIM_EGR_UG;
    TIM15->CR1 |= TIM_CR1_CEN;

    // Stop the timer when the core is halted for debugging
    DBGMCU->APB2FZ |= DBGMCU_APB2_FZ_DBG_TIM15_STOP;
}



int main(void){

    EPL_SystemClock_Config();

    init_tim15();

    NVIC_SetPriority(TIM15_IRQn, 10);
    NVIC_EnableIRQ(TIM15_IRQn);
    __enable_irq();
    for(;;){
        asm("nop");
    }
    return 0;
}





/* For debugging purposes only */
void TIM15_IRQHandler(void) {
    if (TIM15->SR & TIM_SR_UIF){
        TIM15->SR &= ~TIM_SR_UIF;
    }
}