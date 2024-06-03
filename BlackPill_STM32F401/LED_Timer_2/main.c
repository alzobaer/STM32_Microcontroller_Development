#include "RccConfig.h"

void TIM5Config (void);
void Delay_us (uint16_t us);
void Delay_ms (uint16_t ms);
void GPIOConfig (void);

void TIM5Config (void) {
	// steps to follow
	// 1. Enable Timer clock.
	// 2. Set the prescalar and the ARR
	// 3. Enable the Timer, and wait for the update Flag to set
	
	// 1. Enable Timer clock.
	RCC->APB1ENR |= (1<<3); // Enable the timer5 clock
	
	// 2. Set the prescalar and the ARR
	TIM5->PSC = 84-1; // 84MHz/84 = 1 MHz ~~ 1 uS delay
	TIM5->ARR = 0xffff; // MAX ARR value
	
	// 3. Enable the Timer, and wait for the update Flag to set
	TIM5->CR1 |= (1<<0); // Enable the counter
	while(!(TIM5->SR & (1<<0))); // UIF: Update interrupt flag... This bit is set by hardware on an update event. It is cleared by software.
}

void Delay_us (uint16_t us) {
	// steps to follow
	// 1. reset the counter.
	// 2. Wait for the counter to reach the entered value. As each count will take 1 us,
	//		the total waiting time will be the required us delay.
	
	TIM5->CNT = 0;
	while(TIM5->CNT < us);
}

void Delay_ms (uint16_t ms) {
	uint16_t i;
	for (i = 0; i < ms; i++) {
		Delay_us (1000); // delay of 1 ms
	}
}

void GPIOConfig (void) {
	// steps to follow
	// 1. Enable GPIOA clock.
	// 2. Set the pin PA5 as output.
	// 3. Configure the output mode.
	
	// 1. Enable the GPIO Clock.
	RCC->AHB1ENR |= (1<<0);
	
	// 2. Set the Pin as Output.
	GPIOA->MODER |= (1<<10); // pin PA5 (bits 11:10) as Output 01
	
	// 3. Configure the Output Mode.
	// GPIOA->OTYPER &= ~(1<<5); // bit5 = 0 ... for push pull output
	GPIOA->OTYPER = 0;
	GPIOA->OSPEEDR |= (1<<11); // pin PA5 (bits 11:10) as Fast Speed (0:1)
	// GPIOA->PUPDR &= ~((1<<10) | (1<<11)); // pin PA5 (bits 11:10) are 0:0 --> no pull-up or pull-down
	GPIOA->PUPDR = 0;
}

int main(void) {
	SysClockConfig ();
	TIM5Config ();
	GPIOConfig();
	
	while (1) {
		GPIOA->BSRR |= (1<<5); // set the pin PA5
		Delay_ms (1000);
		GPIOA->BSRR |= ((1<<5)<<16); // reset the pin PA5
		Delay_ms (1000);
	}
}

