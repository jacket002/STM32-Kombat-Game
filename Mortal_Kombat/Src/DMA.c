#include "stm32l476xx.h"
#include "DMA.h"

volatile uint16_t joystick_data[2] = { 0, 0 };

void DMA1_Init(void) {

	// Enable clock for DMA1 controller
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;

	// Turn DMA channel off before configuring settings
	DMA1_Channel1->CCR &= ~(DMA_CCR_EN);

	// Total number of transfers per cycle (2 channels, X & Y for joystick data)
	DMA1_Channel1->CNDTR = 2;

	DMA1_Channel1->CCR &= ~(DMA_CCR_DIR);
	DMA1_Channel1->CCR |= DMA_CCR_MINC; // Set memory increment mode
	DMA1_Channel1->CCR |= DMA_CCR_CIRC; // Set DMA channel 1 to circular mode for continuous data conversions

	// Clear and set peripheral data size for 32-bit data transfer
	DMA1_Channel1->CCR &= ~DMA_CCR_PSIZE; // clear PSIZE bits [12:11] mask in CCR register
	DMA1_Channel1->CCR |= DMA_CCR_PSIZE_0; // Set 0x2 (10) in PSIZE bits (32 bit peripheral data)

	// Clear and set memory data size for 32-bit data transfer
	DMA1_Channel1->CCR &= ~DMA_CCR_MSIZE; // clear MSIZE bits [14:13]
	DMA1_Channel1->CCR |= DMA_CCR_MSIZE_0; /// set 0x2 (10) for 32 bit memory data size

	// Enable 2 data transfers (circular mode ensures cycle repeats every time number reaches zero)
	DMA1_Channel1->CNDTR = 2;

	// Source address is peripheral address (ADC1)
	// CPAR (Peripheral Address Register): Holds the base address of the peripheral --> point to ADC1->DR
	DMA1_Channel1->CPAR = (uint32_t) &(ADC1->DR);

	// Destination address is memory
	// CMAR (Memory Address Register): Set base address of the array buffer for the joystick inputs
	DMA1_Channel1->CMAR = (uint32_t) joystick_data;

	DMA1_CSELR->CSELR &= ~DMA_CSELR_C1S; // point channel 1 DMA to ADC1


	DMA1_Channel1->CCR |= DMA_CCR_EN; // DMA1 channel 1 enable

}

