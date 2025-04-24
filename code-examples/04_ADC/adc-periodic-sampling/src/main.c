/*
 * ============================================================================
 *  WARNING: This program is **UNDER DEVELOPMENT**
 * ============================================================================
 *      This code is a work-in-progress.
 *      It is subject to change, refactoring, or complete rewrites at any time.
 *      Expect bugs. Expect weirdness. Expect dragons.
 *
 *  Use at your own risk — it might be terribly broken right now.
 *
 *  You've been warned.
 * ============================================================================
 */

#include "epl.h"
#define ADC_TIMEOUT 100000


void init_adc(void);
void init_tim15(void);
void init_gpio(void);


int main(void){

    EPL_SystemClock_Config(); 

    init_tim15();
    init_gpio();
    init_adc();

    NVIC_SetPriority(TIM15_IRQn, 10);
    NVIC_EnableIRQ(TIM15_IRQn);
    NVIC_EnableIRQ(ADC1_COMP_IRQn); 
    NVIC_SetPriority(ADC1_COMP_IRQn,0); 

    __enable_irq();

    for(;;){
        asm("nop");
    }

    return 0;
}


void init_tim15(void){

    RCC->APB2ENR |= RCC_APB2ENR_TIM15EN;         // Enable clock for TIM15 on APB2 bus

    TIM15->CR1 &= ~(0b1 << 0);                   // Disable TIM15 by clearing the CEN (counter enable) bit

    TIM15->PSC = 48 - 1;                         // Set prescaler to divide clock by 48 (assuming 48 MHz system clock)
    TIM15->ARR = 1000 - 1;                       // Set auto-reload value for 1 ms overflow (1000 ticks @ 48 kHz)

    TIM15->CR1 |= (0b1 << 7);                    // Enable auto-reload preload (ARPE bit)
    TIM15->CR1 |= (0b1 << 2);                    // Enable update request source (URS = 1, only overflow/underflow generate update)

    TIM15->EGR |= (0b1 << 0);                    // Generate an update event to load registers (UG bit)

    TIM15->CR2 |= 0x2 << 4;                      // Configure TRGO (trigger output) to be sent on update event (MMS[2:0] = 010)
    TIM15->SR &= ~(0b1 << 0);                    // Clear the update interrupt flag (UIF bit)
    TIM15->EGR |= (0b1 << 0);                    // Generate an update event again to apply all changes
    TIM15->CR1 |= (0b1 << 0);                    // Start the timer by setting CEN (counter enable) bit

    DBGMCU->APB2FZ |= (0b1 << 16);               // Freeze TIM15 when core is halted during debugging (useful for breakpoints)
}


void init_gpio(void){
    // Enable the GPIOA and ADC clock
    RCC->AHBENR  |= RCC_AHBENR_GPIOBEN; 
    GPIOA->MODER &= (0b11 < 0);  
}

void init_adc(void){ 

    RCC->APB2ENR |= RCC_APB2ENR_ADCEN; 

    // Set the GPIOA pin 0 to analog mode


    ADC1->CFGR1 = 0x00000000;
    ADC1->CFGR2 = 0x00000000;

    ADC1->CFGR1 &= ~ADC_CFGR1_RES;
    ADC1->CFGR2 &= ~ADC_CFGR2_CKMODE;

    // Set the ADC to continuous mode and select scan direction
    ADC1->CHSELR |= ADC_CHSELR_CHSEL0;
    ADC1->CFGR1  &= ~ADC_CFGR1_CONT; 
    ADC1->CFGR1  |= ADC_CFGR1_SCANDIR;
    ADC1->CFGR1 |= ADC_CFGR1_EXTEN_0 | ADC_CFGR1_EXTSEL_2;

    ADC1->SMPR   |= ADC_SMPR_SMP_0;
    ADC1->IER = ADC_IER_EOCIE | ADC_IER_EOSEQIE | ADC_IER_OVRIE;
    
    // If ADC is not ready set the ADC ready bit
    if ((ADC1->ISR & ADC_ISR_ADRDY) != 0){   
        ADC1->ISR |= ADC_ISR_ADRDY; 
    }
    // Enable the ADC
    ADC1->CR |= ADC_CR_ADEN; 
    // Wait for the ADC to be ready
    while ((ADC1->ISR & ADC_ISR_ADRDY) == 0){
    }

    ADC1->CR |= ADC_CR_ADSTART;
}


void ADC1_COMP_IRQHandler(void){
    if (ADC1->ISR & ADC_ISR_EOC){
        uint32_t adc_val = ADC1->DR;

        ADC1->ISR |= ADC_ISR_EOC;
    }
    if (ADC1->ISR & ADC_ISR_EOS){
        ADC1->ISR |= ADC_ISR_EOS;
    }
}

/* For debugging purposes only */
void TIM15_IRQHandler(void) {
    if (TIM15->SR & TIM_SR_UIF){
        TIM15->SR &= ~TIM_SR_UIF;
    }
}