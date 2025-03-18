#include <stm32f091xc.h>
#define OFFSET 16

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
 * GPIO Shield Position: 2
 */
int main(void){   
    const uint8_t gpio_pin = 1;
    
    // Enable clock for GPIOC | [RM] Chapter Reset and clock control (RCC) -> Page 121
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    
    // Set GPIOC pin X as output | [RM] General-purpose I/Os (GPIO) -> Page 158
    GPIOC->MODER |= 0b01 << (2*gpio_pin);
    
    // Set GPIOC pin C as push-pull output | [RM] General-purpose I/Os (GPIO) -> Page 158
    GPIOC->OTYPER &= ~(0b1 << (gpio_pin));

    for(;;){
        delay(100000);
        // Set GPIOC pin X to high | [RM] General-purpose I/Os (GPIO) -> Page 161
        GPIOC->BSRR |= 0b1 << gpio_pin;
        delay(100000);    
        // Set GPIOC pin X to low | [RM] General-purpose I/Os (GPIO) -> Page 161
        GPIOC->BSRR |= 0b1 << (gpio_pin + 16);

    }
}
