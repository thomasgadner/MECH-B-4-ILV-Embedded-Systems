#include <stm32f091xc.h>
#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))


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
    uint8_t gpio_outputPin = 0;
    uint8_t gpio_inputPin = 13;
    
    // Enable clock for GPIOC
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    
    // Set GPIOB pin X as output
    // Set GPIOB pin X as push-pull output
    GPIOB->MODER |= GPIO_MODER_MODER0_0 << 2*gpio_outputPin;
    GPIOB->OTYPER &= ~(GPIO_OTYPER_OT_0 << 2*gpio_outputPin);


    // Set GPIOA pin X as input
    GPIOC->MODER &= ~(GPIO_MODER_MODER0_0 << 2*gpio_inputPin);
    GPIOC->MODER &= ~(GPIO_MODER_MODER0_1 << 2*gpio_inputPin);
    

    uint8_t currentBtn = 0;
    uint8_t previousBtn = 0;
    uint8_t is_led_set = 0;

    for(;;){

        if(((GPIOC->IDR) & (1<<gpio_inputPin))!= 0){
            currentBtn = 1;
        }else{
            currentBtn = 0;
        }

        if ((previousBtn == 0) && (currentBtn ==1)) {

            if(is_led_set){
                // Set GPIOC pin X to low
                GPIOB->BSRR |= GPIO_BSRR_BR_0 << gpio_outputPin;
                is_led_set = 0;
                
            }else{
                // Set GPIOC pin X to high
                GPIOB->BSRR |= GPIO_BSRR_BS_0 << gpio_outputPin;
                is_led_set = 1;
            }
        }
        // Very ugly and poorly working version of a button debounce, but easy to understand.
        delay(5000);
        previousBtn = currentBtn; 

    }
}