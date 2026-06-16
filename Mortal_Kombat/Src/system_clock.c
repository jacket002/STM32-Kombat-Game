#include "stm32l476xx.h"


// Macros
#define __NVIC_PRIO_BITS 4U // define macro for setting NVIC priority
#define SysTick_CTRL_CLKSOURCE_Pos 2U
/*!< SysTick CTRL: CLKSOURCE Position */
#define SysTick_CTRL_CLKSOURCE_Msk (1UL << SysTick_CTRL_CLKSOURCE_Pos)
/*!< SysTick CTRL: CLKSOURCE Mask */
#define SysTick_CTRL_ENABLE_Pos 0U
#define SysTick_CTRL_TICKINT_Pos 1U
/*!< SysTick CTRL: TICKINT Position */
#define SysTick_CTRL_TICKINT_Msk (1UL << SysTick_CTRL_TICKINT_Pos)
/*!< SysTick CTRL: TICKINT Mask */
/*!< SysTick CTRL: ENABLE Position */
#define SysTick_CTRL_ENABLE_Msk (1UL)
/*<< SysTick_CTRL_ENABLE_Pos*/
/*!< SysTick CTRL: ENABLE Mask */

#define RCC_PLLCFGR_PLLN (0x7FUL << 8) /*!< 0x00007F00 */

volatile uint32_t sys_us;

void System_Clock_Init(void) {

	// Set flash wait states
	FLASH->ACR &= ~FLASH_ACR_LATENCY;
	FLASH->ACR |= FLASH_ACR_LATENCY_4WS;

	// Enable the Internal High-Speed oscillator (HSI)
	RCC->CR |= RCC_CR_HSION;
	while((RCC->CR & RCC_CR_HSIRDY) == 0);

	// Calibrate HSI16 in Internal clock sources calibration register (RCC_ICSCR)
	uint32_t HSITrim;
	// Adjusts the Internal High Speed oscillator (HSI) calibration value
	// RC oscillator frequencies are factory calibrated by ST for 1 % accuracy at 25oC
	// After reset, the factory calibration value is loaded in HSICAL[7:0] of RCC_ICSCR
	HSITrim = 16; // user-programmable trimming value that is added to
	//HSICAL[7:0] in ICSCR.
	RCC->ICSCR &= ~RCC_ICSCR_HSITRIM;
	RCC->ICSCR |= HSITrim << 24;

	// Disable the PLL by setting PLLON to 0 in Clock control register (RCC_CR)
	RCC->CR &= ~RCC_CR_PLLON;
	while((RCC->CR & RCC_CR_PLLRDY) == RCC_CR_PLLRDY);
	// Wait until PLLRDY is cleared. The PLL is now fully stopped.

	// Change the desired parameter (PLLM, PLLN,PLLR) in RCC_PLLCFGR.
		// PNNM: Pre-divider for the input clock (can be set from 1 to 8)
		// PLLN: Multiplier for the input clock (can be set from 8 to 86)
		// PLLR: Post-dividers that divide the output of the PLL for different clock domains (e.g., CPU,ADC,USB)

	// Make PLL as 80 MHz
	// f(VCO clock) = f(PLL clock input) * (PLLN / PLLM)
	// = 16MHz * 20/2 = 160 MHz
	// f(PLL_R) = f(VCO clock) / PLLR = 160MHz/2 = 80MHz
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLN;
	RCC->PLLCFGR |= 20U << 8;
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM;
	RCC->PLLCFGR |= 1U << 4;
	// 000: PLLM = 1, 001: PLLM = 2, 010: PLLM = 3, 011: PLLM = 4, 100: \
	PLLM = 5, 101: PLLM = 6, 110: PLLM = 7, 111: PLLM = 8
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLR; // 00: PLLR = 2, 01: PLLR = 4, \
	10: PLLR = 6, 11: PLLR = 8

	RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSI;
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLREN; // Enable PLL output allowing the PLL to provide its clock signal to the system.
	RCC->CR |= RCC_CR_PLLON; // This command enables the PLL.
	while((RCC->CR & RCC_CR_PLLRDY) == 0); //wait for PLL to be ready
	// Select PLL selected as system clock
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_PLL; // 00: MSI, 01:HSI, 10: HSE, 11: PLL
	// Wait until System Clock has been selected
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);


	// Select the PLL clock source in RCC->PLLCFGR.

	// Enable the PLL again by setting PLLON to 1 in RCC->PLLCR

	// Enable the desired PLL outputs by configuring PLLREN, PLLQEN, PLLREN in PLL
	// configuration register (RCC_PLLCFGR)

	// System Clock Selection: Select PLL selected as system clock by ensuring that
	// it is correctly selected in RCC->CFGR.

}

void SysTick_Initialize(int ticks) {

	// disable the SysTick timer
	SysTick->CTRL = 0;

	// Set value in reload register SysTick->LOAD
	SysTick->LOAD = (ticks - 1);

	// Set the interrupt priority
	NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);
	// sets priority of SysTick interrupt to lowest urgency
	/* __NVIC_PRIO_BITS is a macro that defines the number of priority bits
	 *   available in the NVIC  for interrupts. This is specific to the processor
	 *   and can vary depending on the number of  implemented priority levels (4 bits.)
	 *
	 *   */
	// reset the count by setting SysTick->VAL to zero
	SysTick->VAL = 0;

	// configure SysTick->CTRL to use the internal processor clock instead of \
	external clock (1 = processor clock; 0 = external clock)
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;

	// Enables SysTick interrupt, 1 = Enable, 0 = Disable
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;

	// Enable the system timer by setting the ENABLE field in SysTick->CTRL
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;


}

// SysTick interrupt handler
void SysTick_Handler(void) {
	sys_us++;
}

uint32_t micros(void) {
	return sys_us;
}

