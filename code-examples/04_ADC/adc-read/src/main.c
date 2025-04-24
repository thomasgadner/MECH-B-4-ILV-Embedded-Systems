#include <stm32f091xc.h>
#define TIMEOUT 100000




/**
* @brief Check if timeout is reached. This is a function to be used in conjunction with timer_add ()
* @param time Time in cycles to check
* @return 1 if timeout reached 0 if not ( 0 is returned on timeout ) Note : It's not possible to use timer_add () in this
*/
uint8_t timeout(uint32_t time){
    static uint32_t cnt = 0;
    cnt++;
    if (cnt > time){
        return 1;
    }
    return 0;
}


/**
* @brief Error handler for program exit. We don't have a way to tell the user what went wrong
*/
void error(void){
    for(;;){}
}



/**
* @brief Main function for ADC. This is the entry point for the application. It sets up the GPIO and the ADC channels and starts the ADC. ADC is connected to GPIOA pin 0
* @return 0 on success non - zero on failure to initialize the hardware. In case of error the error code is returned
*/
int main(void){ 

    const uint8_t an_shield_pos1 = 0;                      // Constant variable for analog shield position (ADC channel 8 = PB0)

    RCC->AHBENR  |= RCC_AHBENR_GPIOBEN;                    // Enable clock for GPIO port B
    RCC->APB2ENR |= RCC_APB2ENR_ADCEN;                     // Enable clock for ADC1 on APB2 bus

    GPIOB->MODER &= ~(0b11 << an_shield_pos1);             // Set PB0 to analog mode by setting MODER0[1:0]

    ADC1->CHSELR |= 0b1 << 8;                              // Select ADC channel 8 (PB0) in channel selection register
    ADC1->CFGR1  |= 0b1 << 13;                             // Enable continuous conversion mode (CONT bit)
    ADC1->CFGR1  |= 0b1 << 2;                              // Set scan direction (SCANDIR bit)

    ADC1->SMPR   |= 0b1 << 0;                              // Set sample time 

    if ((ADC1->ISR & (0b1 << 0)) != 0){                    // Check if ADC ready flag is set (bit 0 of ISR)
        ADC1->ISR |= (0b1 << 0);                           // Clear ADC ready flag by writing 1 (write-to-clear)
    }

    ADC1->CR |= 0b1 << 0;                                  // Enable ADC by setting ADEN bit in control register

    while ((ADC1->ISR & (0b1 << 0)) == 0){                 // Wait until ADC is ready (ADRDY flag is set)
        if (timeout(TIMEOUT)){                             // Check for timeout during wait
            error();                                       // Handle timeout error
        }
    }

    uint32_t adc_val = 0;                                  // Variable to store the ADC converted value

    ADC1->CR |= 0b1 << 2;                                  // Start ADC conversion by setting ADSTART bit

    for(;;){                                               // Infinite loop to continuously read ADC values
        while ((ADC1->ISR & ADC_ISR_EOC) == 0) {           // Wait for End Of Conversion flag (EOC)
            if (timeout(TIMEOUT)){                         // Check for timeout during wait
                error();                                   // Handle timeout error
            }
        }

        adc_val = ADC1->DR;                                // Read converted ADC value from data register
                                                           // Set breakpoint here to view value in debugger
    }
}








