#include "clock_.h"

/**
 * @brief  System Clock Configuration
 *         The system Clock is configured as follow :
 *            System Clock source            = PLL (HSI48)
 *            SYSCLK(Hz)                     = 48000000
 *            HCLK(Hz)                       = 48000000
 *            AHB Prescaler                  = 1
 *            APB1 Prescaler                 = 1
 *            HSI Frequency(Hz)              = 48000000
 *            Flash Latency(WS)              = 1
 * @param  None
 * @retval None
 */
void SystemClock_Config(void)
{
    // Reset the Flash 'Access Control Register', and
    // then set 1 wait-state and enable the prefetch buffer.
    // (The device header files only show 1 bit for the F0
    //  line, but the reference manual shows 3...)
    FLASH->ACR &= ~(FLASH_ACR_LATENCY_Msk | FLASH_ACR_PRFTBE_Msk);
    FLASH->ACR |= (FLASH_ACR_LATENCY |
                   FLASH_ACR_PRFTBE);

    // activate the internal 48 MHz clock
    RCC->CR2 |= RCC_CR2_HSI48ON;

    // wait for clock to become stable before continuing
    while (!(RCC->CR2 & RCC_CR2_HSI48RDY))
        ;

    // configure the clock switch
    RCC->CFGR = RCC->CFGR & ~RCC_CFGR_HPRE_Msk;
    RCC->CFGR = RCC->CFGR & ~RCC_CFGR_PPRE_Msk;
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW_Msk) | (0b11 << RCC_CFGR_SW_Pos);

    // wait for clock switch to become stable
    while ((RCC->CFGR & RCC_CFGR_SWS) != (0b11 << RCC_CFGR_SWS_Pos))
        ;
}

void Init_Debug_UART()
{

    // Define constants for the USART2 RX and TX pin numbers on GPIOA
    const uint8_t USART2_RX_PIN = 3;
    const uint8_t USART2_TX_PIN = 2;

    // Enable the clock for GPIOA peripheral (used for USART2 pins PA2 and PA3)
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    // Enable the clock for USART2 peripheral
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    // ---------------- UART TX Pin Configuration (PA2) ----------------
    // Set PA2 mode to 'Alternate Function' (MODER bits = 10)
    GPIOA->MODER |= 0b10 << (USART2_TX_PIN * 2);

    // Select AF1 (Alternate Function 1) for PA2 (AFR[0] corresponds to pins 0–7)
    GPIOA->AFR[0] |= 0b0001 << (4 * USART2_TX_PIN);

    // ---------------- UART RX Pin Configuration (PA3) ----------------
    // Set PA3 mode to 'Alternate Function' (MODER bits = 10)
    GPIOA->MODER |= 0b10 << (USART2_RX_PIN * 2);

    // Select AF1 (Alternate Function 1) for PA3
    GPIOA->AFR[0] |= 0b0001 << (4 * USART2_RX_PIN);

    // ---------------- USART2 Configuration ----------------

    // Set the baud rate (BRR = APB frequency divided by desired baud rate)
    // APB_FREQ and BAUDRATE are assumed to be defined elsewhere
    USART2->BRR = (APB_FREQ / BAUDRATE);

    // Enable USART2 receiver by setting RE bit in CR1 (bit 2)
    USART2->CR1 |= 0b1 << 2;

    // Enable USART2 transmitter by setting TE bit in CR1 (bit 3)
    USART2->CR1 |= 0b1 << 3;

    // Enable USART2 by setting UE bit in CR1 (bit 0)
    USART2->CR1 |= 0b1 << 0;
}

/**
 * @brief Delays the program execution for a specified amount of time.
 * @param time The amount of time to delay in number of cycles.
 * @return 0 when the delay is completed.
 */
void delay(uint32_t time)
{
    for (uint32_t i = 0; i < time; i++)
    {
        asm("nop"); // No operation, used for delaying
    }
    return 0;
}

// For supporting printf function we override the _write function to redirect the output to UART
int _write(int handle, char *data, int size)
{
    int count = size;
    while (count--)
    {
        while (!(USART2->ISR & USART_ISR_TXE))
        {
        };
        USART2->TDR = *data++;
    }
    return size;
}
