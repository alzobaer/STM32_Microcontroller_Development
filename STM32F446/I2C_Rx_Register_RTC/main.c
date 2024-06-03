#include "stm32f4xx.h"

#define I2C_ADDRESS_DS3231   0x68  // I2C address for DS3231 RTC module

void I2C_Init(void) {
    // Enable GPIOB clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

    // Configure PB8 and PB9 for I2C1 (Alternate function, open-drain, pull-up)
    GPIOB->MODER &= ~(GPIO_MODER_MODE8 | GPIO_MODER_MODE9);
    GPIOB->MODER |= (GPIO_MODER_MODE8_1 | GPIO_MODER_MODE9_1); // Alternate function mode
    GPIOB->OTYPER |= (GPIO_OTYPER_OT8 | GPIO_OTYPER_OT9); // Open-drain
    GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD8 | GPIO_PUPDR_PUPD9);
    GPIOB->PUPDR |= (GPIO_PUPDR_PUPD8_0 | GPIO_PUPDR_PUPD9_0); // Pull-up
    GPIOB->AFR[1] |= (4 << GPIO_AFRH_AFSEL8_Pos) | (4 << GPIO_AFRH_AFSEL9_Pos); // AF4 for I2C1

    // Enable I2C1 clock
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

    // Reset I2C1
    RCC->APB1RSTR |= RCC_APB1RSTR_I2C1RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_I2C1RST;

    // Configure I2C1
    I2C1->CR1 = 0; // Clear PE bit (disable I2C)
    I2C1->CR2 = 16; // Set peripheral clock frequency (16 MHz)
    I2C1->CCR = 80; // Configure clock control register (100 kHz standard mode)
    I2C1->TRISE = 17; // Configure maximum rise time
    I2C1->CR1 |= I2C_CR1_ACK; // Enable ACK
    I2C1->CR1 |= I2C_CR1_PE; // Enable I2C
}

uint8_t I2C_ReadRegister(uint8_t deviceAddr, uint8_t regAddr) {
    uint8_t data;

    // Wait until I2C is not busy
    while (I2C1->SR2 & I2C_SR2_BUSY);

    // Start I2C communication
    I2C1->CR1 |= I2C_CR1_START;

    // Wait for start bit generation
    while (!(I2C1->SR1 & I2C_SR1_SB));

    // Send the address with the write bit
    I2C1->DR = (deviceAddr << 1) & 0xFE;

    // Wait for address to be sent
    while (!(I2C1->SR1 & I2C_SR1_ADDR));

    // Clear ADDR flag by reading SR1 and SR2
    (void)I2C1->SR1;
    (void)I2C1->SR2;

    // Send the register address
    I2C1->DR = regAddr;

    // Wait for register address to be sent
    while (!(I2C1->SR1 & I2C_SR1_TXE));

    // Repeat start condition and send the address with the read bit
    I2C1->CR1 |= I2C_CR1_START;

    // Wait for start bit generation
    while (!(I2C1->SR1 & I2C_SR1_SB));

    // Send the address with the read bit
    I2C1->DR = (deviceAddr << 1) | 0x01;

    // Wait for address to be sent
    while (!(I2C1->SR1 & I2C_SR1_ADDR));

    // Clear ADDR flag by reading SR1 and SR2
    (void)I2C1->SR1;
    (void)I2C1->SR2;

    // Wait for data to be received
    while (!(I2C1->SR1 & I2C_SR1_RXNE));

    // Read the data
    data = I2C1->DR;

    // Stop I2C communication
    I2C1->CR1 |= I2C_CR1_STOP;

    return data;
}

uint8_t BCD_to_Decimal(uint8_t bcd) {
    return ((bcd / 16 * 10) + (bcd % 16));
}

void read_DS3231_time(uint8_t* hours, uint8_t* minutes, uint8_t* seconds) {
    uint8_t seconds_bcd = I2C_ReadRegister(I2C_ADDRESS_DS3231, 0x00); // Read seconds register
    uint8_t minutes_bcd = I2C_ReadRegister(I2C_ADDRESS_DS3231, 0x01); // Read minutes register
    uint8_t hours_bcd = I2C_ReadRegister(I2C_ADDRESS_DS3231, 0x02);   // Read hours register

    *seconds = BCD_to_Decimal(seconds_bcd);
    *minutes = BCD_to_Decimal(minutes_bcd);
    *hours = BCD_to_Decimal(hours_bcd);
}

int main(void) {
    I2C_Init();

    uint8_t hours, minutes, seconds;
    while (1) {
        read_DS3231_time(&hours, &minutes, &seconds);
        // Now 'hours', 'minutes', and 'seconds' contain the current time from the RTC
        // You can add your own logic here to display or process the time as needed

        for (volatile int i = 0; i < 1000000; i++); // Simple delay
    }
}
