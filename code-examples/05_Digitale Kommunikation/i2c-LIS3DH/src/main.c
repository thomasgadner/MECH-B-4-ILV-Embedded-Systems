/*******************************************************************************
 * Final Course Example - Important Note
 *
 * Dear Students,
 *
 * As we reach the conclusion of our journey together in this course,
 * this example does not include any direct page references to the 
 * microcontroller's datasheet.
 *
 * By now, through your exercises and practice, you should have developed
 * the confidence and ability to navigate the datasheet independently. 
 * This is an essential skill for every embedded developer, and we trust 
 * that your hard work has brought you to that level.
 *
 * Remember: the datasheet is not just a reference—it's your guidebook.
 * Take pride in how far you’ve come, and let this be one more step toward
 * becoming a skilled and self-reliant engineer.
 *
 * All the best,
 ******************************************************************************/


#include <stm32f091xc.h>
#include "clock_.h"
#include <stdio.h>

// This is a simple macro to print debug messages of DEBUG is defined
#define DEBUG
#ifdef DEBUG
#define LOG(msg...) printf(msg);
#else
#define LOG(msg...) ;
#endif

// Define GPIO PINs for I2C
#define PIN_SDA 9
#define PIN_SCL 8

// The LIS3DH Address is a 7 bit address + 1 bit for read/write
// The LSB of the address can be changed by setting SDO low or high (0b001100(SDO)) -> See Page 25 of Sensor Datasheet.
#define SDO 0
#define LIS3DH_BASE_ADDR (0b0011000 | SDO)
#define LIS3DH_ADDR LIS3DH_BASE_ADDR << 1

/**
 * @brief Writes data to a target device via I2C communication.
 *
 * This function writes data to a target device using the I2C protocol. It configures the I2C controller,
 * sets the target address, sends the data, and waits for the transmission to complete.
 *
 * @param targetAddr The 7-bit address of the target device.
 * @param buf Pointer to the buffer containing the data to be sent.
 * @param len Number of bytes to send.
 * @return 0 on success.
 */
int8_t I2C_Controller_Write(uint8_t targetAddr, uint8_t *buf, size_t len)
{

    // Reset the CR2 Control Register
    I2C1->CR2 = 0x00000000;

    // Check if the I2C bus is busy
    while ((I2C1->ISR & (0b1<<15)) == (0b1<<15)) 
        ;

    // Set the target address and number of bytes to send
    I2C1->CR2 |= (targetAddr + 0x0) << 0;
    I2C1->CR2 |= (len) << 16;

    // Start the transmission
    I2C1->CR2 |= 0b1<<13;

    // Send the data
    for (uint8_t i = 0; i < len; i++)
    {
        if ((I2C1->ISR & (0b1<<0)) == (0b1<<0))
        {
            I2C1->TXDR = buf[i];
        }
        // Wait for the TXE to complete to send the next byte
        while (!((I2C1->ISR & (0b1<<0)) == (0b1<<0)))
            ;
    }

    // Check if the transmission is complete
    while (!((I2C1->ISR & (0b1<<6)) == (0b1<<6)))
        ;

    // Stop the transmission
    I2C1->CR2 |= 0b1<<14;

    // Wait for the bus to be free
    while ((I2C1->ISR & (0b1<<15)) == (0b1<<15))
        ;
    I2C1->ICR |= 0b1<<5;

    return 0;
}

/**
 * @brief Reads data from a target device via I2C communication.
 *
 * This function reads data from a target device using the I2C protocol. It configures the I2C controller,
 * sets the target address, sends the register address to read from, receives the data, and waits for the
 * reception to complete.
 *
 * @param targetAddr The 7-bit address of the target device.
 * @param reg The register address to read from.
 * @param data Pointer to the buffer where the received data will be stored.
 * @param len Number of bytes to read.
 * @return 0 on success.
 */
int8_t I2C_Target_Read(uint8_t targetAddr, uint8_t reg, uint8_t *data, size_t len)
{

    uint8_t count = 0;

    // Reset the CR2 Control Register
    I2C1->CR2 = 0x00000000;
    // Set the target address and number of bytes to read
    I2C1->CR2 |= (targetAddr) << 0; // Move Target (Slave) Address to SADD. P. 689
    I2C1->CR2 |= 1 << 16;           // Set the NBYTES Field to send and recv. 1 Byte

    // Start the transmission
    I2C1->CR2 |= 0b1 << 13; // Start the I2C by setting the START Bit in CR2

    // Send the register address to read from
    while (!((I2C1->ISR & (0b1 << 0)) == (0b1 << 0)))
        ;
    I2C1->TXDR = (uint8_t)reg;
    while (!((I2C1->ISR & (0b1 << 6)) == (0b1 << 6)))
        ;

    // Reset the CR2 Control Register
    I2C1->CR2 = 0x00000000;

    // Set the target address and number of bytes to read, and set the read bit
    I2C1->CR2 |= (targetAddr + 0x1) << 0;
    I2C1->CR2 |= (len) << 16;
    // Put the controller in read mode
    I2C1->CR2 |= (0b1 << 10); // Put the controller in Master Mode
    // Enable automatic end mode
    I2C1->CR2 |= 0b1 << 25; // Select the Autoend Mode

    // Start the transmission
    I2C1->CR2 |= 0b1 << 13; // Start the Transmission

    // Receive the data
    while (count < len)
    {
        while (!((I2C1->ISR & (0b1 << 2)) == (0b1 << 2)))
            ;
        data[count++] = I2C1->RXDR; // Send Data by putting it into the RXDR Reg.
    }

    // Autoend will stop the transmission automaticaly

    return 0;
}

/**
 * @brief Writes data to a register of the LIS3DH sensor via I2C communication.
 *
 * This function writes data to a specific register of the LIS3DH sensor using the I2C protocol. It constructs
 * a buffer containing the register address and data, then calls the I2C_Controller_Write function to perform
 * the actual write operation.
 *
 * @param targetAddr The 7-bit address of the LIS3DH sensor.
 * @param reg The register address to write to.
 * @param data The data to write into the specified register.
 */
void LIS3DH_Write(uint8_t targetAddr, uint8_t reg, uint8_t data)
{
    uint8_t buf[2];
    buf[0] = reg;
    buf[1] = data;
    I2C_Controller_Write(targetAddr, buf, 2);
}

/**
 * @brief Reads data from a register of the LIS3DH sensor via I2C communication.
 *
 * This function reads data from a specific register of the LIS3DH sensor using the I2C protocol. It calls the
 * I2C_Target_Read function to perform the read operation.
 *
 * @param targetAddr The 7-bit address of the LIS3DH sensor.
 * @param reg The register address to read from.
 * @param buffer Pointer to the buffer where the received data will be stored.
 * @param size Number of bytes to read.
 */
void LIS3DH_Read(uint8_t targetAddr, uint8_t reg, uint8_t *buffer, uint8_t size)
{
    I2C_Target_Read(targetAddr, reg, buffer, size);
}

/**
 * @brief Configures the I2C peripheral and GPIO pins for I2C communication.
 *
 * This function configures the I2C peripheral and GPIO pins for I2C communication. It enables the necessary
 * GPIO and I2C clock, sets the pin modes, types, speeds, and pull-up resistors. Additionally, it configures
 * the GPIO alternate function registers and sets the I2C timing register. Finally, it enables the I2C peripheral.
 */
void I2C_config()
{

    // Enable the GPIOB and I2C1 clock
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

    // Set the GPIO pin modes to alternate function
    GPIOB->MODER |= 0b10 << (PIN_SDA << 1);
    GPIOB->MODER |= 0b10 << (PIN_SCL << 1);

    // I2C Needs open-drain
    GPIOB->OTYPER |= 0b1 << (PIN_SDA) | 0b1 << (PIN_SCL);

    // GPIO Needs to be high speed
    GPIOB->OSPEEDR |= 0b11 << (PIN_SDA << 1);
    GPIOB->OSPEEDR |= 0b11 << (PIN_SCL << 1);

    // Enable pull-up resistors
    GPIOB->PUPDR |= 0b01 << (PIN_SDA << 1);
    GPIOB->PUPDR |= 0b01 << (PIN_SCL << 1);

    // Set the GPIO pins to alternate function 1
    GPIOB->AFR[PIN_SDA / 8] |= 0b0001 << ((PIN_SDA % 8) * 4);
    GPIOB->AFR[PIN_SCL / 8] |= 0b0001 << ((PIN_SCL % 8) * 4);

    // Set the I2C timing register - please refer to the Datasheet for more information
    I2C1->TIMINGR = (uint32_t)0x00B01A4B; // Table 95 Page 666. Calculations and the Register can be found on Page 691.
    // Enable the I2C peripheral
    I2C1->CR1 |= (0b1 << 0); // enable the peripheral by setting the Peripheral Enable (PE) bit.
}

/**
 * @brief Initializes the LIS3DH sensor.
 *
 *  @returns 1 if the sensor's whoami register responded correctly, else 0
 *
 *  */
int LIS3DH_Init()
{
    uint8_t check[] = {0x00};

    // Read the WHO_AM_I register to verify communication with the sensor
    LIS3DH_Read(LIS3DH_ADDR, 0x0F, check, 1);

    // Give the sensor some time
    delay(APB_FREQ / 100);

    // Check if the sensor responded with the correct value
    if (check[0] == 0x33)
    {
        LIS3DH_Write(LIS3DH_ADDR, 0x20, 0b01010111);
        LIS3DH_Write(LIS3DH_ADDR, 0x23, 0b00001000);

        return 1;
    }
    return 0;
}

/**
 * @brief Sets the LIS3DH's SDO pin
 *
 */
void set_SDO(int state)
{
    const uint8_t sdo_pin = 4;

    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    GPIOB->MODER |= 0b1 << (2 * sdo_pin);
    GPIOB->OTYPER &= ~(0b1 << (2 * sdo_pin));
    if (state)
    {
        GPIOB->BSRR |= (0b1) << sdo_pin;
    }
    else
    {
        GPIOB->BSRR |= 0b1 << (sdo_pin+15);
    }
}

int main(void)
{

    // Initialize the system clock and debug UART
    SystemClock_Config();
    Init_Debug_UART();

    // Configure the I2C peripheral
    I2C_config();

    // Initialize the LIS3DH sensor
    set_SDO(SDO);
    while (!LIS3DH_Init())
    {
        ;
    }

    int16_t GX = 0;
    int16_t GY = 0;
    int16_t GZ = 0;

    uint8_t rx_data[6];

    // Main loop
    for (;;)
    {
        // Read the gyroscope data
        for (int ro = 0; ro < 6; ro++)
        {
            LIS3DH_Read(LIS3DH_ADDR, 0x28 + ro, rx_data + ro, 1);
        }

        // Convert the raw data to 16-bit signed values
        GX = (int16_t)((rx_data[1] << 8) | rx_data[0]);
        GY = (int16_t)((rx_data[3] << 8) | rx_data[2]);
        GZ = (int16_t)((rx_data[5] << 8) | rx_data[4]);

        // Send scaled data to debug UART

        LOG(">GX:%.4d\n",GX);
        LOG(">GY:%.4d\n",GY);
        LOG(">GZ:%.4d\n",GZ);

        // Wait for a short delay
        delay(APB_FREQ / 5000);
    }

    return 0;
}
