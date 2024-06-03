#include "stm32f401xc.h"

void SysClockConfig(void);
void GPIO_Config(void);
void Delay(uint32_t time);

void SysClockConfig(void) {
	/*STEP#1 enable hse and wait for ready */
	RCC->CR |= RCC_CR_HSEON;       /* set RCC_CR register 16th bit 1 */
	while(!(RCC->CR & (1 << 17))); /* wait until RCC_CR register 17th bit is 1 */
	
	/*STEP#2 setup power eccnable clock and voltage regulator */
	RCC->APB1ENR |= 1 << 28;  /* make 28th bit to 1 */
	PWR->CR |= 2 << 14;
	
	/* STEP#3 Flash Prefetch Register & Latency*/
	FLASH->ACR = 1 << 8 | 1 << 9 | 1 << 10 | 2 << 0;
	
	/* 
	 ** STEP#4 Configure HCLK (main system clock), PCLK1, PCLK2 
	*/
	RCC->CFGR &= ~(1U << 4);
	RCC->CFGR |= (4 << 10);
	RCC->CFGR &= ~(1U << 13);
	 
	/*step#5 ENABLE PLL*/
	#define PLL_M 3
	#define PLL_N 84
	#define PLL_P 0
	
	RCC->PLLCFGR = (PLL_P << 16) | (PLL_M << 0) | (PLL_N << 6) | (1 << 22);
	
	/* STEP#6: Enalbe PLL and wait for ready */
	RCC->CR |= (1 << 24);
	while(!(RCC->CR & (1 << 25)));
	
	/* STEP # 7: Select the Clock Source and wait for it to be set */
	RCC->CFGR |= (2 << 0);
	while(!(RCC->CFGR & (2 << 2)));
}

void GPIO_Config(void) {
	/* Enable GPIO Clock */
	/* set the pin for output */
	/* Configure the Output mode */
	RCC->AHB1ENR |= (1 << 0);
	GPIOA->MODER |= (1 << 10);
	GPIOA->OTYPER |= 0; /* low */
	GPIOA->OSPEEDR |= 0;
}

void Delay(uint32_t time) {
	while(time--);
}

int main(void) {
	SysClockConfig();
	GPIO_Config();
	
	while(1) {
			GPIOA->BSRR |= (1 << 5); /* set the pin5 */
			Delay(1000000);
			GPIOA->BSRR |= ((1 << 5) << 16); /* reset pin*/
			Delay(1000000);
	}
	
}

