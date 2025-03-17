#include <stm32f091xc.h>


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
 * @brief Main function where the program execution starts.
 */
int main(void){   
    uint8_t gpio_pin = 1;
    
    // Enable clock for GPIOC
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    
    // Set GPIOC pin X as output
    GPIOC->MODER |= GPIO_MODER_MODER0_0 << (2*gpio_pin);
    
    // Set GPIOC pin C as push-pull output
    GPIOC->OTYPER &= ~(GPIO_OTYPER_OT_0 << (2*gpio_pin));

    for(;;){
        // Set GPIOC pin X to high
        GPIOC->BSRR |= GPIO_BSRR_BS_0 << gpio_pin;
        
        // Delay for 100000 cycles
        delay(100000);
        
        // Set GPIOC pin X to low
        GPIOC->BSRR |= GPIO_BSRR_BR_0 << gpio_pin;

        // Delay for 100000 cycles
        delay(100000);
    }
}
