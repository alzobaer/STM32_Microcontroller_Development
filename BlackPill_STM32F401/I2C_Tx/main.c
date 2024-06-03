#include "RccConfig.h"
#include "Delay.h"

void I2C_Config (void);
void I2C_Start(void);
void I2C_Write(uint8_t data);
void I2C_Address(uint8_t Address);
void I2C_Stop(void);
void I2C_WriteMulti(uint8_t *data, uint8_t size);

void I2C_Config (void) {
	// Steps to be followed
	// 1. Enable the I2C CLOCK and GPIO CLOCK
	// 2. Configure the I2C PINs for Alternate Functions
	//		a) Select Alternate Function in MODER Register
	//		b) Select Open Drain Output
	// 		c) Select High SPEED for the PINs
	//		d) Select Pull-up for both the Pins
	// 		e) Configure the Alternate Function in AFR Register
	//	3. Reset the I2C
	//	4. Program the peripheral input clock in I2C_CR2 Register in order to generate correct timings
	//	5. Configure the clock control register
	//	6. Configure the rise time register
	//	7. Program the I2C_CR1 register to enable the peripheral

	
	// Enable the I2C CLOCK and GPIO CLOCK
	RCC->APB1ENR |= (1<<21); // enable I2C CLOCK
	RCC->AHB1ENR |= (1<<1); // enable GPIOB CLOCK
	
	// Configure the I2C PINs for Alternate Functions
	GPIOB->MODER |= (2<<12) | (2<<14); // Bits (13:12) = 1:0 --> Alternate Function for pin PB6; Bits (15:14) = 1:0 --> Alternate Function for pin PB7
	GPIOB->OTYPER |= (1<<6) | (1<<7); // Bit6=1, Bit7=1 output open drain
	GPIOB->OSPEEDR |= (3<<12) | (3<<14); // Bits (13:12) = 1:1 --> High Speed for pin PB6; Bits (15:14) = 1:1 --> High Speed for pin PB7
	GPIOB->PUPDR |= (1<<12) | (1<<14); // Bits (13:12) = 0:1 --> Pull up for pin PB6; Bits (15:14) = 0:1 --> Pull up for pin PB7
	GPIOB->AFR[0] |= (4<<24) | (4<<28); // Bits (3:2:1:0) = 0:1:0:0 --> AF4 for pin PB6; Bits (7:6:5:4) = 0:1:0:0 --> AF4 for pin PB7
	
	// Reset the I2C
	I2C1->CR1 |= (1<<15);
	I2C1->CR1 &= ~(1<<15);
	
	// Program the peripheral input clock in I2C_CR2 Register in order to generate correct timings
	I2C1->CR2 |= (42<<0); // PCLK1 Frequency in MHz
	
	// Configure the clock control register
	I2C1->CCR = (210<<0); // calculated value for 42 MHz and 1000 ns, 4 us
												// CCR = (Tr(scl) + Tw(sclh)) / Tpclk1
												// Tpclk1 = 1 / Frequency
		
	// Configure the rise time register
	I2C1->TRISE = 43; // calculated value for 42 MHz and 1000 ns
										// Trise = Tr(scl) / Tpclk1 + 1
										// Tpclk1 = 1 / Frequency
	
	// Program the I2C_CR1 register to enable the peripheral
	I2C1->CR1 |= (1<<0); // Enable I2C
	
}

void I2C_Start(void){
	// steps followed....
	// 1. send the START condition
	// 2. wait for the SB (Bit 0 in SR1) to set. This indicates that the start condition is generated

	I2C1->CR1 |= (1 << 8);	// generate start
	while(!(I2C1->SR1 & (1 << 0)));	// wait for SB bit to set
	
}

void I2C_Write(uint8_t data){
	// steps followed:
	// 1. wait for the TXE (bit 7 in SR1) to set. This indicates that the DR is empty
	// 2. send the DATA to the DR register
	// 3. wait for the BTF(Byte Transfer Finished) (bit 2 in SR1) to set. this indicates the end of LAST DATA transmission
	
	while(!(I2C1->SR1 & (1<<7)));	// wait for TXE (Data register empty (transmitters)) bit is set
	I2C1->DR = data;
	while(!(I2C1->SR1 & (1<<2)));	// wait for BTF bit is set
}

void I2C_Address(uint8_t Address){
	// STEPS FOLLOWED
	// 1. send the slave address to the DR register
	// 2. wait for the ADDR (bit 1 in SR1) to set. this indicates the end of the address transmission
	// 3. clear the ADDR by reading the SR1 and SR2
	
	I2C1->DR = Address;	// send the address
	while(!(I2C1->SR1 & (1<<1)));	// wait for ADDR bit to set
	uint8_t temp = I2C1->SR1 | I2C1->SR2;	// read SR1 and SR2 to clear the ADDR bit	
}

void I2C_Stop(void){
	I2C1->CR1 |= (1<<9);	// stop I2C
}

void I2C_WriteMulti(uint8_t *data, uint8_t size){
	// STEPS FOLLOWED
	// 1. Wait for the TXE (bit 7 in SR1) to set. This indicates the DR is empty
	// 2. Keep sending DATA to the DR register after performing the check if the TXE bit is set
	// 3. Once the DATA transfer is completed, wait for the BTF(bit 2 in SR1) to set.
	
	while(!(I2C1->SR1 & (1 << 7)));	// wait for TXE bit to set
	while(size){
		while(!(I2C1->SR1 & (1<<7)));	// wait for TXE bit to set
		I2C1->DR = (volatile uint8_t)*data++; // send data
		size--;
	}
	while(!(I2C1->SR1 & (1<<2)));	// wait for BTF to set
}


int main (void) {
	
	SysClockConfig();
	TIM5Config();
	I2C_Config();
	
	I2C_Start();
	I2C_Address(0x4E);
	I2C_Write(0x02);
	I2C_Stop();

	while (1) {
	
	
	}
	
}

