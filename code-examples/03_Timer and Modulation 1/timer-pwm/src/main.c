/******************************************************************************
 * @brief  Configure system and peripherals for PWM generation using TIM3
 *
 * @details
 * - System clock initialization
 * - Enable GPIOC and TIM3 peripheral clocks
 * - Configure GPIOC pin for alternate function (PWM output)
 * - Set TIM3 for center-aligned PWM mode
 * - Set prescaler and ARR for desired PWM frequency
 * - Configure TIM3 Channel 3 in PWM Mode 1
 * - Enable channel output and start the timer
 ******************************************************************************/

#include "main.h"
#include "clock_.h"

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

    const uint8_t pwm_shield_position_1 = 8;

    EPL_SystemClock_Config();

    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; 


    GPIOC->MODER |= 0b10 << pwm_shield_position_1*2;
    GPIOC->OSPEEDR |= 0b11 << pwm_shield_position_1*2;

    // [RM] General-purpose I/Os Page: 163
    GPIOC->AFR[1] |= 0b0000; // Set Pin into AFM 0
  
    TIM3->CR1 &= ~0b1<<0; // Disable the timer
    // [RM] General-purpose timers (TIM2 and TIM3) Page: 457    
    TIM3->CR1 |= 0b01 << 5; // Select the Center Aligned Mode
    
    // Set the prescaler and overflow values
    TIM3->PSC = 0;
    TIM3->ARR = 47999/2;

    // [RM] General-purpose timers (TIM2 and TIM3) Page: 464
    TIM3->CCR3 = 0; // Set CC Value to 0

    // [RM] General-purpose timers (TIM2 and TIM3) Page: 457
    TIM3->CCMR2 &= ~0b11 << 0; // Make sure Channel 3 is an Output
    TIM3->CCMR2 |= 0b110 << 4; // Put this System in PWM Mode 1

    // [RM] General-purpose timers (TIM2 and TIM3) Page: 461
    TIM3->CCER |= 0b1 << 8 ; // Enable the CC Channel  3

    TIM3->CR1 |= 0b1<<0; // Enable the timer
   
    for(;;){
        for (int duty_cycle = 0; duty_cycle < TIM3->ARR; duty_cycle++){
            TIM3->CCR3 = duty_cycle;
            delay(1000);
        }

        for (int duty_cycle = TIM3->ARR; duty_cycle >= 0; duty_cycle--){
            TIM3->CCR3 = duty_cycle;
            delay(1000);
        }
    }
}