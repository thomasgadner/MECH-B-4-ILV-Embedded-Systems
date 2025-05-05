#include <stm32f091xc.h>
#include "mci.h"
#include <stdio.h>


// This is a simple macro to print debug messages of DEBUG is defined
#define DEBUG
#ifdef DEBUG
  #define LOG( msg... ) printf( msg );
#else
  #define LOG( msg... ) ;
#endif


// Define GPIO PINs for I2C
#define PIN_SDA 9
#define PIN_SCL 8


// The LIS3DH Address is a 7 bit address + 1 bit for read/write
// The LSB of the address can be changed by setting SDO low or high (0b001100(SDO))
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
int8_t I2C_Controller_Write(uint8_t targetAddr, uint8_t* buf, size_t len){

    // Reset the CR2 Control Register
    I2C1->CR2 = 0x00000000;

    // Check if the I2C bus is busy
    while((I2C1->ISR & I2C_ISR_BUSY) == I2C_ISR_BUSY);

    // Set the target address and number of bytes to send
    I2C1->CR2 |= (targetAddr + 0x0) << I2C_CR2_SADD_Pos;
    I2C1->CR2 |= (len) << I2C_CR2_NBYTES_Pos; 

    // Start the transmission
    I2C1->CR2 |= I2C_CR2_START;
    
    // Send the data
    for(uint8_t i = 0; i < len; i++){
        if ((I2C1->ISR & I2C_ISR_TXE) == (I2C_ISR_TXE)){
            I2C1->TXDR = buf[i];
        }
        // Wait for the TXE to complete to send the next byte
        while(!((I2C1->ISR & I2C_ISR_TXE) == (I2C_ISR_TXE)));
    }

    // Check if the transmission is complete
    while(!((I2C1->ISR & I2C_ISR_TC) == (I2C_ISR_TC))); 

    // Stop the transmission
    I2C1->CR2 |= I2C_CR2_STOP;

    // Wait for the bus to be free
    while((I2C1->ISR & I2C_ISR_BUSY) == I2C_ISR_BUSY);
    I2C1->ICR |= I2C_ICR_STOPCF;

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
int8_t I2C_Target_Read(uint8_t targetAddr, uint8_t reg, uint8_t *data, size_t len){

    uint8_t count = 0;

    // Reset the CR2 Control Register
    I2C1->CR2 = 0x00000000;
    //Set the target address and number of bytes to read
    I2C1->CR2 |= (targetAddr) << I2C_CR2_SADD_Pos;
    I2C1->CR2 |= 1 << I2C_CR2_NBYTES_Pos; 

    // Start the transmission
    I2C1->CR2 |= I2C_CR2_START; 

    // Send the register address to read from
    while(!((I2C1->ISR & I2C_ISR_TXE) == (I2C_ISR_TXE))); 
    I2C1->TXDR = (uint8_t)reg;
    while(!((I2C1->ISR & I2C_ISR_TC) == (I2C_ISR_TC))); 

    // Reset the CR2 Control Register
    I2C1->CR2 = 0x00000000;

    // Set the target address and number of bytes to read, and set the read bit
    I2C1->CR2 |= (targetAddr + 0x1) << I2C_CR2_SADD_Pos;
    I2C1->CR2 |= (len)<<I2C_CR2_NBYTES_Pos; 
    // Put the controller in read mode
    I2C1->CR2 |= (I2C_CR2_RD_WRN); 
    // Enable automatic end mode
    I2C1->CR2 |= I2C_CR2_AUTOEND; 
 
    // Start the transmission
    I2C1->CR2 |= I2C_CR2_START; 
    
    // Receive the data
    while(count < len){
        while(!((I2C1->ISR & I2C_ISR_RXNE) == (I2C_ISR_RXNE)));
        data[count++]= I2C1->RXDR;
    }

    // Autoend will stop the transmission

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
void LIS3DH_Write (uint8_t targetAddr, uint8_t reg, uint8_t data){
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
void LIS3DH_Read (uint8_t targetAddr, uint8_t reg, uint8_t *buffer, uint8_t size){
    I2C_Target_Read(targetAddr, reg, buffer, size);
}


/**
 * @brief Configures the I2C peripheral and GPIO pins for I2C communication.
 *
 * This function configures the I2C peripheral and GPIO pins for I2C communication. It enables the necessary
 * GPIO and I2C clock, sets the pin modes, types, speeds, and pull-up resistors. Additionally, it configures
 * the GPIO alternate function registers and sets the I2C timing register. Finally, it enables the I2C peripheral.
 */
void I2C_config(){

    // Enable the GPIOB and I2C1 clock
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

    // Set the GPIO pin modes to alternate function
    GPIOB->MODER |= 0b10 << (PIN_SDA<<1);
    GPIOB->MODER |= 0b10 << (PIN_SCL<<1); 

    // I2C Needs open-drain
    GPIOB->OTYPER |=  1 << (PIN_SDA) | 1 << (PIN_SCL);

    // GPIO Needs to be high speed
    GPIOB->OSPEEDR |= 0b11 << (PIN_SDA << 1);
    GPIOB->OSPEEDR |= 0b11 << (PIN_SCL << 1);


    // Enable pull-up resistors
    GPIOB->PUPDR |= 0b01 << (PIN_SDA << 1);
    GPIOB->PUPDR |= 0b01 << (PIN_SCL << 1);

    // Set the GPIO pins to alternate function 1
    GPIOB->AFR[PIN_SDA / 8] |= 0b0001 << ((PIN_SDA % 8) * 4); 
    GPIOB->AFR[PIN_SCL / 8] |= 0b0001 << ((PIN_SCL % 8) * 4); 

    // Set the I2C timing register - please refer to the datasheet for more information
    I2C1->TIMINGR =  (uint32_t)0x00B01A4B;
    // Enable the I2C peripheral
    I2C1->CR1 |= I2C_CR1_PE; //enable peripheral

}



/**
 * @brief Initializes the LIS3DH sensor.
 *
 *  @returns 1 if the sensor's whoami register responded correctly, else 0
 * 
 *  */
int LIS3DH_Init(){
    uint8_t check[] = {0x00};

    // Read the WHO_AM_I register to verify communication with the sensor
    LIS3DH_Read(LIS3DH_ADDR, 0x0F, check, 1);

    // Give the sensor some time
    EPL_delay(APB_FREQ/100);

    // Check if the sensor responded with the correct value
    if(check[0] == 0x33){
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
void set_SDO(int state){
    uint8_t sdo_pin = 4;

    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    GPIOB->MODER |= GPIO_MODER_MODER0_0 << (2*sdo_pin);
    GPIOB->OTYPER &= ~(GPIO_OTYPER_OT_0 << (2*sdo_pin));
    if (state) {GPIOB->BSRR |= GPIO_BSRR_BS_0 << sdo_pin;}
    else {GPIOB->BSRR |= GPIO_BSRR_BR_0 << sdo_pin;}
}

int main(void){

    // Initialize the system clock and debug UART
    EPL_SystemClock_Config();
    EPL_init_Debug_UART();

    // Configure the I2C peripheral
    I2C_config();

    // Initialize the LIS3DH sensor
    set_SDO(SDO);
    while (!LIS3DH_Init()){;}


    int16_t GX = 0;
    int16_t GY = 0;
    int16_t GZ = 0;

    uint8_t rx_data[6];

    // Main loop
    for(;;){
        // Read the gyroscope data
        for (int ro = 0; ro < 6; ro++){
            LIS3DH_Read(LIS3DH_ADDR, 0x28 + ro, rx_data + ro, 1);
        }

        // Convert the raw data to 16-bit signed values
        GX = (int16_t)((rx_data[1] << 8) | rx_data[0]);
        GY = (int16_t)((rx_data[3] << 8) | rx_data[2]);
        GZ = (int16_t)((rx_data[5] << 8) | rx_data[4]);

        // Send scaled data to debug UART
        LOG("[DEBUG-LOG] AX: %.4d | AY: %.4d | AZ: %.4d\r\n", GX, GY, GZ);

        // Wait for a short delay
        EPL_delay(APB_FREQ/5000);

    }

    return 0;
}

