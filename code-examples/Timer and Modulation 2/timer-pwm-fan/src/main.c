#include "main.h"
#include "mci_clock.h"

#define C


/**
 * @brief Delays the program execution for a specified amount of time.
 * @param time The amount of time to delay in number of cycles.
 * @return 0 when the delay is completed.
 */
int delay(uint32_t time){
    for(uint32_t i = 0; i < time; i++ ){
        asm("nop"); // No operation, used for delaying
    }
    return 0;
}


/**
 * @brief Main function
 * @return 0
 */
int main(void){
    EPL_SystemClock_Config();

    // Enable the GPIO A,B,C and TIM3 peripherals
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; 

    // Set GPIOB pin 0 HIGH
    GPIOB->MODER |= GPIO_MODER_MODER0_0;
    GPIOB->OTYPER &= ~GPIO_OTYPER_OT_0;
    GPIOB->BSRR |= GPIO_BSRR_BS_0;

    // Set GPIOA pin 4 to LOW
    GPIOA->MODER |= GPIO_MODER_MODER0_0 << 2*4;
    GPIOA->OTYPER &= ~GPIO_OTYPER_OT_0 << 2*4;
    GPIOA->BSRR |= GPIO_BSRR_BR_0 << 4;

    // Set the mode of the GPIOC pin 8 to alternate function mode 0
    GPIOC->MODER |= 0b10 << 8*2;
    GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR5;
    GPIOC->AFR[1] |= 0b0000;
    
    // Disable the timer before setting the prescaler and overflow values
    TIM3->CR1 &= ~TIM_CR1_CEN;
    
    // Set the prescaler and overflow values
    TIM3->PSC = 0;
    TIM3->ARR = 47999/2;
    TIM3->CCR3 = 0;

    // Set the PWM mode 1
    TIM3->CCMR2 |= TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1;
    TIM3->CCMR1 |= TIM_CCMR2_OC3PE;

    // Set the output polarity to active high
    TIM3->CCER |= TIM_CCER_CC3E;

    // Enbable the timer
    // Set the center-aligned mode 
    TIM3->CR1 |= TIM_CR1_CMS_0 | TIM_CR1_CEN;
    
    TIM3->CCR3 = 0;

    for (;;){
        for (int duty = 0; duty < TIM3->ARR; duty++){
            TIM3->CCR3 = duty;
            delay(250);
        }

        for (int duty = TIM3->ARR; duty >= 0; duty--){
            TIM3->CCR3 = duty;
            delay(250);
        }

    }
}