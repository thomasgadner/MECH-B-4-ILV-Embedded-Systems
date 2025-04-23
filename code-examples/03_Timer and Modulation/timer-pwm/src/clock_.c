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
void EPL_SystemClock_Config(void)
{

    FLASH->ACR &= ~0b111; // Reset 
    FLASH->ACR |=  0b001 << 0; // Include 1 Wait State
    FLASH->ACR |=  0b1 << 4; // Enable the Prefetch Buffer


    // activate the internal 48 MHz clock
    RCC->CR2 |= 0b1 << 16;;

    // wait for clock to become stable before continuing
    while (!(RCC->CR2 & (0b1 << 17)));

    // configure the clock switch
    RCC->CFGR &= ~0b1111 << 4;
    RCC->CFGR &= ~0b111 << 8;

    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW_Msk) | (0b11 << RCC_CFGR_SW_Pos);

    // wait for clock switch to become stable
    while((RCC->CFGR & RCC_CFGR_SWS) != (0b11 << RCC_CFGR_SWS_Pos))
        ;
}
