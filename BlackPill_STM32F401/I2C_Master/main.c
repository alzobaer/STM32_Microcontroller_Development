#include "stm32f4xx.h"

// Define I2C address of the slave PCF8574 module
#define SLAVE_ADDRESS 0x4E

// Initialize I2C for master communication
void I2C_Master_Init(void) {
    // Enable I2C clock
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
    
    // Configure GPIOB pins for I2C SDA (PB7) and SCL (PB6)
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    GPIOB->MODER |= GPIO_MODER_MODE6_1 | GPIO_MODER_MODE7_1;
    GPIOB->OTYPER |= GPIO_OTYPER_OT6 | GPIO_OTYPER_OT7;
    GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED6 | GPIO_OSPEEDR_OSPEED7;
    GPIOB->AFR[0] |= (4 << (6 * 4)) | (4 << (7 * 4)); // AF4 for PB6 and PB7
    
    // Configure I2C peripheral
    I2C1->CR1 &= ~I2C_CR1_PE;
    I2C1->CR1 |= I2C_CR1_SWRST;
    I2C1->CR1 &= ~I2C_CR1_SWRST;
    I2C1->CR2 |= 42; // Set peripheral clock to 42MHz
    I2C1->CCR |= 210; // Set clock control register
    I2C1->TRISE = 43; // Set maximum rise time
    I2C1->CR1 |= I2C_CR1_PE; // Enable I2C peripheral
}

// Write data to slave
void I2C_Master_Write(uint8_t data) {
    // Send start condition
    I2C1->CR1 |= I2C_CR1_START;
    while (!(I2C1->SR1 & I2C_SR1_SB)); // Wait for start condition to be generated
    
    // Send slave address and write bit
    I2C1->DR = (SLAVE_ADDRESS << 1) | 0; // Slave address + write bit
    while (!(I2C1->SR1 & I2C_SR1_ADDR)); // Wait for address sent
    I2C1->SR2; // Clear ADDR bit
    
    // Send data
    while (!(I2C1->SR1 & I2C_SR1_TXE)); // Wait until data register is empty
    I2C1->DR = data; // Send data
    while (!(I2C1->SR1 & I2C_SR1_BTF)); // Wait until data transfer is complete
    
    // Send stop condition
    I2C1->CR1 |= I2C_CR1_STOP;
}

int main(void) {
    // Initialize I2C
    I2C_Master_Init();
    
    // Turn on LED connected to PCF8574 module
    I2C_Master_Write(0x01); // Write 0xFF to turn on all pins of PCF8574
    
    while (1) {
        // You can add additional functionality here if needed
    }
}
