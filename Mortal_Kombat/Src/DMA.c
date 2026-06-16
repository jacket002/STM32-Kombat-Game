#include "stm32l476xx.h"
#include "DMA.h"

volatile uint32_t joystick_data[2] = {0, 0};

void DMA_Init(void) {

	// Enable clock for DMA1 controller
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;

	// Turn DMA channel off before configuring settings
	DMA1_Channel1->CCR &= ~(DMA_CCR_EN);

	// CPAR (Peripheral Address Register): Holds the base address of the peripheral --> point to ADC1->DR
	DMA1_Channel1->CPAR = (uint32_t)&(ADC1->DR);

	// CMAR (Memory Address Register): Set base address of the array buffer for the joystick inputs
	DMA1_Channel1->CMAR = (uint32_t)joystick_data;

	// Total number of transfers per cycle (2 channels, X & Y for joystick data)
	DMA1_Channel1->CNDTR = 2;

	DMA1_Channel1->CCR |= DMA_CCR_MINC;
	DMA1_Channel1->CCR |= DMA_CCR_CIRC;

	DMA1_Channel1->CCR |= DMA_CCR_EN;

}
