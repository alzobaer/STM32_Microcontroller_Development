#include<stm32f10x.h>

int main(void){
	
	//enabling rcc for GPIO Port C
	RCC->APB2ENR |= 1U<<4;
	
	//enabling rcc for GPIO Port B
	RCC->APB2ENR |= 1U<<3;
	
	//setting Mode for PC13
	GPIOC->CRH |=(1U<<20);
	GPIOC->CRH |=(1U<<21);
	//push pull conf for PC13
	GPIOC->CRH &=~(1U<<22);
	GPIOC->CRH &=~(1U<<23);
	
	//setting Mode for PB12
	GPIOB->CRH |=(1U<<16);
	GPIOB->CRH |=(1U<<17);
	//Open drain conf for PB12
	GPIOB->CRH |=(1U<<18);
	GPIOB->CRH &=~(1U<<19);

	while(1){
		
		// Blinking Using BSRR
		GPIOC->BSRR |= 1U<<13;
		// Blinking USing ODR
		GPIOB->ODR |=(1U<<12);
		for(int i=0;i<1000000;i++){}
		
		GPIOC->BSRR |= (1U<<29);
		GPIOB->BRR |=(1U<<12);
		for(int i=0;i<1000000;i++){}

	}



}
