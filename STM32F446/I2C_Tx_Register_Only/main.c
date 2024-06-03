#include "RccConfig.h"
#include "Delay.h"

void I2C_Config (void);
void I2C_Start (void);
void I2C_Write (uint8_t data);
void I2C_Address (uint8_t Address);
void I2C_Stop (void);
void I2C_WriteMulti (uint8_t *data, uint8_t size);

void I2C_Config (void)
{
/**** STEPS FOLLOWED  ************
1. Enable the I2C CLOCK and GPIO CLOCK
2. Configure the I2C PINs for ALternate Functions
	a) Select Alternate Function in MODER Register
	b) Select Open Drain Output 
	c) Select High SPEED for the PINs
	d) Select Pull-up for both the Pins
	e) Configure the Alternate Function in AFR Register
3. Reset the I2C 	
4. Program the peripheral input clock in I2C_CR2 Register in order to generate correct timings
5. Configure the clock control registers
6. Configure the rise time register
7. Program the I2C_CR1 register to enable the peripheral
*/
	
	// Enable the I2C CLOCK and GPIO CLOCK
	RCC->APB1ENR |= (1<<21);  // enable I2C CLOCK
	RCC->AHB1ENR |= (1<<1);  // Enable GPIOB CLOCK
	
	
	// Configure the I2C PINs for ALternate Functions
	GPIOB->MODER |= (2<<16) | (2<<18);  // Bits (17:16)= 1:0 --> Alternate Function for Pin PB8; Bits (19:18)= 1:0 --> Alternate Function for Pin PB9
	GPIOB->OTYPER |= (1<<8) | (1<<9);  //  Bit8=1, Bit9=1  output open drain
	GPIOB->OSPEEDR |= (3<<16) | (3<<18);  // Bits (17:16)= 1:1 --> High Speed for PIN PB8; Bits (19:18)= 1:1 --> High Speed for PIN PB9
	GPIOB->PUPDR |= (1<<16) | (1<<18);  // Bits (17:16)= 0:1 --> Pull up for PIN PB8; Bits (19:18)= 0:1 --> pull up for PIN PB9
	GPIOB->AFR[1] |= (4<<0) | (4<<4);  // Bits (3:2:1:0) = 0:1:0:0 --> AF4 for pin PB8;  Bits (7:6:5:4) = 0:1:0:0 --> AF4 for pin PB9
	
	
	// Reset the I2C 
	I2C1->CR1 |= (1<<15);
	I2C1->CR1 &= ~(1<<15);
	
	// Program the peripheral input clock in I2C_CR2 Register in order to generate correct timings
	I2C1->CR2 |= (45<<0);  // PCLK1 FREQUENCY in MHz
	
	// Configure the clock control registers
	I2C1->CCR = 225<<0;  // check calculation in PDF
	
	// Configure the rise time register
	I2C1->TRISE = 46;  // check PDF again
	
	// Program the I2C_CR1 register to enable the peripheral
	I2C1->CR1 |= (1<<0);  // Enable I2C
}

void I2C_Start (void)
{
/**** STEPS FOLLOWED  ************
1. Send the START condition 
2. Wait for the SB ( Bit 0 in SR1) to set. This indicates that the start condition is generated
*/	

	//I2C1->CR1 |= (1<<10);  // Enable the ACK
	I2C1->CR1 |= (1<<8);  // Generate START
	while (!(I2C1->SR1 & (1<<0)));  // Wait fror SB bit to set
}

void I2C_Write (uint8_t data)
{
/**** STEPS FOLLOWED  ************
1. Wait for the TXE (bit 7 in SR1) to set. This indicates that the DR is empty
2. Send the DATA to the DR Register
3. Wait for the BTF (bit 2 in SR1) to set. This indicates the end of LAST DATA transmission
*/	
	while (!(I2C1->SR1 & (1<<7)));  // wait for TXE bit to set
	I2C1->DR = data;
	while (!(I2C1->SR1 & (1<<2)));  // wait for BTF bit to set
}

void I2C_Address (uint8_t Address)
{
/**** STEPS FOLLOWED  ************
1. Send the Slave Address to the DR Register
2. Wait for the ADDR (bit 1 in SR1) to set. This indicates the end of address transmission
3. clear the ADDR by reading the SR1 and SR2
*/	
	I2C1->DR = Address;  //  send the address
	while (!(I2C1->SR1 & (1<<1)));  // wait for ADDR bit to set
	uint8_t temp = I2C1->SR1 | I2C1->SR2;  // read SR1 and SR2 to clear the ADDR bit
}

void I2C_Stop (void)
{
	I2C1->CR1 |= (1<<9);  // Stop I2C
}

void I2C_WriteMulti (uint8_t *data, uint8_t size)
{
/**** STEPS FOLLOWED  ************
1. Wait for the TXE (bit 7 in SR1) to set. This indicates that the DR is empty
2. Keep Sending DATA to the DR Register after performing the check if the TXE bit is set
3. Once the DATA transfer is complete, Wait for the BTF (bit 2 in SR1) to set. This indicates the end of LAST DATA transmission
*/	
	while (!(I2C1->SR1 & (1<<7)));  // wait for TXE bit to set 
	while (size)
	{
		while (!(I2C1->SR1 & (1<<7)));  // wait for TXE bit to set 
		I2C1->DR = (uint32_t )*data++;  // send data
		size--;
	}
	
	while (!(I2C1->SR1 & (1<<2)));  // wait for BTF to set
}

int main ()
{
	SysClockConfig ();
	TIM6Config ();
	I2C_Config ();	
	
	I2C_Start ();
	I2C_Address (0x4E);
	I2C_Write (0xff);
	I2C_Stop ();
	
	while (1)
	{
		// Delay_ms (1000);
	}
}

