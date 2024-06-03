#include "stm32f401xc.h"

void SysClockConfig (void);
void GPIO_Config (void);
void delay (uint32_t time);

void SysClockConfig (void) {
	
	/* steps to be followed
		1. Enable HSE and wait for the HSE to become Ready
		2. Set the POWER ENABLE CLOCK and VOLTAGE REGULATOR
		3. Configure the FLASH PREFETCH and LATENCY Related settings.
		4. Configure the PRESCALARS HCLK, PCLK1, PCLK2
		5. Configure the MAIN PLL
		6. Enable the PLL and wait for it to become ready
		7. Select the Clock Source and wait for it to be set
	*/
	
	#define PLL_M 	4
	#define PLL_N 	84
	#define PLL_P		0 // PLLP = 2
	
	// 1. Enable HSE and wait for the HSE to become Ready 
	RCC->CR |= RCC_CR_HSEON;
	while (! (RCC->CR & RCC_CR_HSERDY));
	
	// 2. Set the POWER ENABLE CLOCK and VOLTAGE REGULATOR
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	PWR->CR |= PWR_CR_VOS;
	
	// 3. Configure the FLASH PREFETCH and LATENCY Related settings.
	FLASH->ACR |= FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN | FLASH_ACR_LATENCY_2WS;
	
	// 4. Configure the PRESCALARS HCLK, PCLK1, PCLK2
	// AHB PR
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
	
	// APB1 PR
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
	
	// APB2 PR
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;
	
	// 5. Configure the MAIN PLL
	RCC->PLLCFGR |= (PLL_M << 0) | (PLL_N << 6) | (PLL_P << 16) | (RCC_PLLCFGR_PLLSRC_HSE);
	
	// 6. Enable the PLL and wait for it to become ready
	RCC->CR |= RCC_CR_PLLON;
	while(!(RCC->CR & RCC_CR_PLLRDY));
	
	// 7. Select the Clock Source and wait for it to be set
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);

}

void GPIO_Config (void) {
	/*
		steps
		1. Enable the GPIO Clock.
		2. Set the Pin as Output.
		3. Configure the Output Mode.
	*/
	
	// 1. Enable the GPIO Clock.
	RCC->AHB1ENR |= (1<<0);
	
	// 2. Set the Pin as Output.
	GPIOA->MODER |= (1<<10); // pin PA5 (bits 10:11) as Output 01
	
	// 3. Configure the Output Mode.
	GPIOA->OTYPER = 0;
	GPIOA->OSPEEDR = 0;
}

void delay (uint32_t time) {
	while (time--);
}

int main(void) {
	
	SysClockConfig ();
	GPIO_Config ();
	
	while(1) {
		GPIOA->BSRR |= (1<<5); // set the pin PA5
		delay(8000000);
		GPIOA->BSRR |= ((1<<5)<<16); // reset the pin PA5
		delay(8000000);
	}	
}
