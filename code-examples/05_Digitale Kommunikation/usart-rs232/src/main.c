#include <stm32f0xx.h>
#include "mci_clock.h"
#include <stdio.h>

#define LOG( msg... ) printf( msg );

// Select the Baudrate for the UART
#define BAUDRATE 9600


// For supporting printf function we override the _write function to redirect the output to UART
int _write(int handle, char* data, int size) {
  // 'handle' is typically ignored in this context, as we're redirecting all output to USART2
  // 'data' is a pointer to the buffer containing the data to be sent
  // 'size' is the number of bytes to send

  int count = size;  // Make a copy of the size to use in the loop

  // Loop through each byte in the data buffer
  while (count--) {
      // Wait until the transmit data register (TDR) is empty,
      // indicating that USART2 is ready to send a new byte
      while (!(USART2->ISR & USART_ISR_TXE)) {
          // Wait here (busy wait) until TXE (Transmit Data Register Empty) flag is set
      }

      // Load the next byte of data into the transmit data register (TDR)
      // This sends the byte over UART
      USART2->TDR = *data++;

      // The pointer 'data' is incremented to point to the next byte to send
  }

  // Return the total number of bytes that were written
  return size;
}

int main(void) {
  // Configure the system clock to 48MHz (defined in a separate function)
  EPL_SystemClock_Config();

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

  // Variable to store received byte
  uint8_t rxb = '\0';

  // Infinite loop: continuously receive data and print it
  for (;;) {
      // Wait until the RXNE (Receive Not Empty) flag is set (bit 5 in ISR)
      // This indicates that data has been received and is ready to be read
      while (!(USART2->ISR & (0b1 << 5))) {
          // Wait for data to be received
      }

      // Read the received byte from the RDR (Receive Data Register)
      rxb = USART2->RDR;

      // Log the received byte using printf via the LOG macro
      // The printf function is supported through the custom _write() function,
      // which redirects output to UART (USART2)
      LOG("[DEBUG-LOG]: %d\r\n", rxb);
  }
}
