#include <stm32f091xc.h>
#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))


const uint8_t gpio_inputPin = 13;


// Put a breakpoint here on line 16 to see the interrupt.
void EXTI4_15_IRQHandler(void)     
// We find this function weakly declared in the Startup Code
{
  if (EXTI->PR & (1 << gpio_inputPin)) {
    // Clear the EXTI status flag.

    // [RM] Interrupts and events -> Page 224
    EXTI->PR |= (1 << gpio_inputPin);
  }
 
}



/**
 * @brief Main function where the program execution starts.
 * Push the Blue User Button to trigger the Interrupt
 */
int main(void){   

    // Enable clock for SYSCFG
    // [RM] Reset and clock control (RCC) -> Page 123
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    // Enable clock for GPIOC
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

    // Set GPIOC pin X as input
    GPIOC->MODER &= ~(0b01 << 2*gpio_inputPin);
    GPIOC->PUPDR |= (0b10 << 2*gpio_inputPin);
    

    // Configure the external interrupt configuration register, for the selected pin.
    // [RM] System configuration controller (SYSCFG) -> Page 171
    SYSCFG->EXTICR[3] |= 0b0010 << 4;
    // Set the interrupt mask register to enable the interrupt for the selected pin. 
    // [RM] Interrupts and events -> Page 222  // Read from Page 217 to understand
    EXTI->IMR |= 0b1 << gpio_inputPin;
    // Set the interrupt trigger selection register to select the rising edge trigger for the selected pin.
    // [RM] Interrupts and events -> Page 222 and 223
    EXTI->RTSR |= 0b1 << gpio_inputPin;
    EXTI->FTSR &= ~0b1 << gpio_inputPin;

    // Enable the interrupt for the selected pin.
    // Recall the Lecture Notes ! 
    NVIC_SetPriority(EXTI4_15_IRQn, 0x3);
    NVIC_EnableIRQ(EXTI4_15_IRQn);
    

    for(;;){
        asm("nop");
    }
}




