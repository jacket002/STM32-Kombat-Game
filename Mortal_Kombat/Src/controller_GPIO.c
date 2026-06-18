#include "controller_GPIO.h"
#include "stm32l476xx.h"

void controller_Init(void) {
	// 1. Enable clock for GPIOA
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

	// Set PA4 (X_right_ to digital input mode (00)
	GPIOA->MODER &= ~(3UL << GPIO_MODER_MODE4_Pos);

	// Set PA5 (X_left) to digital input mode (00)
	GPIOA->MODER &= ~(3UL << GPIO_MODER_MODE5_Pos);

	// Set PA6 (Y_jump) to digital input mode (00)
	GPIOA->MODER &= ~(3UL << GPIO_MODER_MODE6_Pos);

	// Set PA7 (strike) to digital input mode (00)
	GPIOA->MODER &= ~(3UL << GPIO_MODER_MODE7_Pos);

	// 4. Reset Pull-Up/Pull-Down bits for all pins and explicitly set to Pull-Up (01)
	GPIOA->PUPDR &= ~((3UL << GPIO_PUPDR_PUPD4_Pos) | (3UL << GPIO_PUPDR_PUPD5_Pos) | (3UL << GPIO_PUPDR_PUPD6_Pos) | (3UL << GPIO_PUPDR_PUPD7_Pos));
	GPIOA->PUPDR |= ((1UL << GPIO_PUPDR_PUPD4_Pos) | (1UL << GPIO_PUPDR_PUPD5_Pos) | (1UL << GPIO_PUPDR_PUPD6_Pos) | (1UL << GPIO_PUPDR_PUPD7_Pos));

}



