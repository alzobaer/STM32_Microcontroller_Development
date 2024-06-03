#include "RccConfig.h"
#include "Delay.h"

void Uart2Config (void);
void UART2_SendChar (uint8_t c);
void UART2_SendString (char *string);
uint8_t UART2_GetChar (void);

void Uart2Config (void) {
	
	// steps to be followed
	// 1. Enable the UART CLOCK and GPIO CLOCK
	// 2. Configure the UART PINs for Alternate Functions
	// 3. Enable the USART by writing the UE bit in USART_CR1 register to 1.
	// 4. Program the M bit in USART_CR1 to define the word length.
	// 5. Select the desired baud rate using the USART_BRR register.
	// 6. Enable the Transmitter/Receiver by Setting the TE and RE bits in USART_CR1 Register.
	
	// 1. Enable the UART CLOCK and GPIO CLOCK
	RCC->APB1ENR |= (1<<17); // Enable UART2 Clock
	RCC->AHB1ENR |= (1<<0); // Enable GPIOA Clock
	
	// 2. Configure the UART PINs for Alternate Functions
	GPIOA->MODER |= (2<<4); // Bits (5:4) = 1:0 --> Alternate Function for Pin PA2
	GPIOA->MODER |= (2<<6); // Bits (7:6) = 1:0 --> Alternate Function for Pin PA3
	
	GPIOA->OSPEEDR |= (3<<4) | (3<<6); // Bits (5:4) = 1:1 and Bits (7:6) = 1:1 --> High Speed for pin PA2 and PA3
	
	GPIOA->AFR[0] |= (7<<8); // Bits (11:10:9:8) = (0:1:1:1) --> AF7 Alternate function for USART2 at Pin PA2
	GPIOA->AFR[0] |= (7<<12); // Bits (15:14:13:12) = (0:1:1:1) --> AF7 Alternate function for USART2 at Pin PA3
	
	// 3. Enable the USART by writing the UE bit in USART_CR1 register to 1.
	USART2->CR1 = 0x00; // clear all --> reset
	USART2->CR1 |= (1<<13); // UE = 1 ... Enable USART
	
	// 4. Program the M bit in USART_CR1 to define the word length.
	USART2->CR1 &= ~(1<<12); // M = 0; 8 bit word length
	
	// 5. Select the desired baud rate using the USART_BRR register.
	// USART2->BRR = 4375;							// Buad rate of 14400, PCLK1 at 63 MHz    (63000000 / 14400 = 4375)
	// USART2->BRR = 547;							// Buad rate of 115200, PCLK1 at 63 MHz    (63000000 / 115200 = 547)
	USART2->BRR = (3<<0) | (34<<4); // Buad rate of 115200, PCLK1 at 63 MHz   (63000000 / (16*115200) = 34.17968)
	
	// 6. Enable the Transmitter/Receiver by Setting the TE and RE bits in USART_CR1 Register.
	USART2->CR1 |= (1<<2); // RE = 1 ... Enable the Receiver
	USART2->CR1 |= (1<<3); // TE = 1 ... Enable the Transmitter

}

void UART2_SendChar (uint8_t c) {

	// Steps to be followed
	// 1. Write the data to send in the USART_DR register (this clears the TXE bit). Repeat this 
	// 		for each data to be transmitted in case of single buffer.
	// 2. After writing the last data into the USART_DR register, wait until TC = 1. This indicates
	// 		that the transmission of the last frame is complete. This is required for instance when
	//		the USART is disabled or enters the Halt mode to avoid corruption the last transmission.
	
	USART2->DR = c; // load the data into DR register.
	while (!(USART2->SR & (1<<6))); // wait for TC to SET...This indicates that the data has been transmitted
	
}

void UART2_SendString (char *string) {
	
	while (*string) {
		UART2_SendChar(*string++);
	}
}

uint8_t UART2_GetChar (void) {
	// Steps followed
	// 1. Wait for the RXNE bit to set. It indicates that the data has been received and can be read.
	// 2. Read the data from USART_DR Register. This also clears the RXNE bit.
	
	uint8_t temp;
	
	while (!(USART2->SR & (1<<5))); // wait for RXNE bit to set.
	temp = USART2->DR; // Read the data. This clears RXNE also.
	return temp;
}

int main (void) {

	SysClockConfig ();
	TIM5Config ();
	Uart2Config ();
	
	while (1) {
		// First we have to check the data send for single character. Then data send for string.
		// After that we have to check the data receive using the hercules(sendind data).
		// The data send and receive section will be check seperately - commenting one while checking other.
		
		// Data Send
		// UART2_SendChar ('G'); // single character is send
		UART2_SendString ("\nData is sending from MCU to PC\n");
		Delay_ms(3000);	// 3sec
		
		// Receive Data
		uint8_t data = UART2_GetChar ();
		UART2_SendChar (data);
	}
	
}
