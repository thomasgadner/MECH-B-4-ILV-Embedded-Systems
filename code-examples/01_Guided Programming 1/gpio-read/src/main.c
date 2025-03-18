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
 * Shield Position: 2, The Pushbutton PIN 13 Port is the blue button on the Nucleo Board.
 */
int main(void){   
    uint8_t gpio_outputPin = 1;
    uint8_t gpio_inputPin = 13;
    
    // Enable clock for GPIOC
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    
    // Set GPIOC pin X as output
    // Set GPIOC pin X as push-pull output
    GPIOC->MODER |= 0b01 << 2*gpio_outputPin;
    GPIOC->OTYPER &= ~(0b1 << gpio_outputPin );
    
    // Set GPIOC pin X as input
    GPIOC->MODER &= ~(0b11 << 2*gpio_inputPin);

    

    uint8_t currentBtn = 0;
    uint8_t previousBtn = 0;
    uint8_t is_led_set = 0;

    for(;;){

        // [RM] General-purpose I/Os (GPIO) -> Page 160
        if(((GPIOC->IDR) & (1<<gpio_inputPin))!= 0){
            currentBtn = 1;
        }else{
            currentBtn = 0;
        }

        if ((previousBtn == 0) && (currentBtn ==1)) {

            if(is_led_set){
                // Set GPIOC pin X to low
                GPIOC->BSRR |= 0b1 << gpio_outputPin;
                is_led_set = 0;
                
            }else{
                // Set GPIOC pin X to high
                GPIOC->BSRR |= 0b1 << (gpio_outputPin+16);
                is_led_set = 1;
            }
        }
        // Very ugly and poorly working version of a button debounce, but easy to understand.
        delay(5000);
        previousBtn = currentBtn; 
    }
}