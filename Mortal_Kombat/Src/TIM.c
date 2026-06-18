#include <TIM.h>
#include "stm32l476xx.h"

volatile uint32_t msTicks;
volatile uint32_t secondsElapsed = 0;
volatile uint32_t toggle = 0;

void BUZZER_PIN_INIT(void) {

	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN; // enable clock for port A

	GPIOA->MODER &= ~(GPIO_MODER_MODE0); // clear
	GPIOA->MODER |= (GPIO_MODER_MODE0_1); // set PA0 as alternate function (10)

	/* Set alternate function PA0 to TIM2_CH1
	(Port A TIM2_CH1 uses AF1. PA0 is the lower echelon of pin bits, so TIM5 will use AFRL, AF2 (AFR[0])  */
	GPIOA->AFR[0] &= ~(0xF << (0 * 4));  // clear PA0 bit field
	GPIOA->AFR[0] |= (1 << (0 * 4)); // Set AF1 (TIM2)

}

/* Create a function void TIM2_Init(); to configure the TIM2 timer to run at a period of 10ms
(0.01s) */
void TIM2_Init(void) {

	// Enable the TIM2 clock in the RCC->APB1ENR1 register
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
	// Set TIM2-CR1 DIr to up-counting
	TIM2->CR1 &= ~TIM_CR1_DIR;
	//Enable ARR preload (ARPE)

	TIM2->PSC = 0;
	TIM2->ARR = 163635;

	// Set TIM2->CCMR1 OC1M field to PWM Mode 1 (0b110)
	TIM2->CCMR1 &= ~TIM_CCMR1_OC1M; // Clear mode bits
	TIM2->CCMR1 |= (6 << TIM_CCMR1_OC1M_Pos); // Set to PWM Mode 1
	TIM2->CCMR1 |= TIM_CCMR1_OC1PE; // enable preload register
	// Set TIM2->CCER output polarity to active high
	TIM2->CCER = 1;
	// Set TIM2->CCER CCIE field to enable channel 1 output
	TIM2->CCER |= TIM_CCER_CC1E;
	// Set TIM2->CR1 to enable the counter
	TIM2->CR1 |= TIM_CR1_CEN;
	// Set TIM2->CCR1 to achieve a 50% duty cycle
	TIM2->CCR1 = (TIM2->ARR)/2;

}

// Sets TIM3 which kicks off an ISR that decides when buzzer should be on or OFF
void TIM3_Init(void) {

	// Enable the TIM3 clock in the RCC->APB1ENR1 register
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM3EN;

	TIM3->PSC = 7199;
	// To get 125ms: 1250 ticks. ARR = 1250 - 1 = 1249
	TIM3->ARR = 1249;
	// Set TIM2->CCMR1 OC1M field to PWM Mode 1 (0b110)

	// Enable update interrupts
	TIM3->DIER |= TIM_DIER_UIE;
	NVIC_EnableIRQ(TIM3_IRQn); // Enable TIm3 interrupt in NVIC
	TIM3->CR1 |= TIM_CR1_CEN; // enable timer

}

void TIM15_Init() {

	// Enable the TIM15 clock in the APB2ENR
	RCC->APB2ENR |= RCC_APB2ENR_TIM15EN;

	// Divide 80 MHz to 10 kHz
	TIM15->PSC = 7999;
    // Count 10000 for 1 Hz (1 second)
	TIM15->ARR = 9999;

	// Enable update interrupts
	TIM15->DIER |= TIM_DIER_UIE;
	NVIC_EnableIRQ(TIM1_BRK_TIM15_IRQn); // Enable TIM15 interrupt in NVIC

	NVIC_SetPriority(TIM1_BRK_TIM15_IRQn, 2);

	TIM15->CR1 |= TIM_CR1_CEN; // enable match countdown (TIM15)

}

void TIM3_IRQHandler(void) {

	// Check if update interrupt flag is set
		if (TIM3->SR & TIM_SR_UIF) {
			TIM3->SR &= ~TIM_SR_UIF; // clear the interrupt flag

			if (toggle == 1) {
				playSong();

				if (TIM2->ARR != 0) {
					TIM2->CCER |= TIM_CCER_CC1E;

				}
			} else {
				TIM2->CCER &= ~TIM_CCER_CC1E;

			}

			toggle = !toggle;

		}


}

void TIM1_BRK_TIM15_IRQHandler(void) {

	// Check if update interrupt flag is set
		if (TIM15->SR & TIM_SR_UIF) {
			TIM15->SR &= ~TIM_SR_UIF; // clear the interrupt flag

			secondsElapsed++;

		}


}

