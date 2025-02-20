#include "main.h"
#include "mci_clock.h"

#define C

#define NOTE_E  659 // E5
#define NOTE_B  987 // B5
#define NOTE_C  1047 // C6
#define NOTE_D  1175 // D6
#define NOTE_F  1397 // F6
#define NOTE_A  880 // A5
#define NOTE_G  784 // G5

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

uint32_t arr_from_freq(uint16_t freq){
    return 48000000/(freq*(TIM3->PSC+1)) - 1;
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
    TIM3->ARR = arr_from_freq(NOTE_E);
    TIM3->CCR3 = 0;

    // Set the PWM mode 1
    TIM3->CCMR2 |= TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1;
    TIM3->CCMR1 |= TIM_CCMR2_OC3PE;

    // Set the output polarity to active high
    TIM3->CCER |= TIM_CCER_CC3E;

    // Enbable the timer
    // Set the center-aligned mode 
    TIM3->CR1 |= TIM_CR1_CMS_0 | TIM_CR1_CEN;
    
    TIM3->CCR3 = TIM3->ARR/2;

    int frequencies[] = {
        NOTE_E, NOTE_B, NOTE_C, NOTE_D, 
        NOTE_C, NOTE_B, NOTE_A, NOTE_A, 
        NOTE_C, NOTE_E, NOTE_D, NOTE_C, 
        NOTE_B, NOTE_B, NOTE_C, NOTE_D,
        NOTE_E, NOTE_C, NOTE_A
    };

    int noteLengths[] = {
        2, 1, 1, 2, 
        1, 1, 2, 1, 
        1, 2, 1, 1, 
        2, 1, 1, 2,
        2, 2, 2, 2,
    };

    for (;;){
        for (int note = 0; note < 19; note++){
            TIM3->ARR = arr_from_freq(frequencies[note])/2;
            delay(1000000 * noteLengths[note]);
        }

        delay(1000000 * 5);
    }


}