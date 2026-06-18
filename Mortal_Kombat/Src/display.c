#include "stm32l476xx.h"
#include "display.h"

// Initialize I2C1 for communication with the LCD screen.
// Configuration is controller mode, fast mode, with the peripheral address set to 0x3C.
void I2C_Init(void) {
	// Enable clock to port B
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;

	// FIX: Read from GPIOB->MODER, not GPIOA->MODER!
	GPIOB->MODER = (GPIOB->MODER & ~(GPIO_MODER_MODE8 | GPIO_MODER_MODE9))
			| (0b10UL << GPIO_MODER_MODE8_Pos)
			| (0b10UL << GPIO_MODER_MODE9_Pos);

	// FIX: Read from GPIOB->AFR[1], not GPIOA->AFR[1]!
	GPIOB->AFR[1] = (GPIOB->AFR[1] & ~(GPIO_AFRH_AFSEL8 | GPIO_AFRH_AFSEL9))
			| (4UL << GPIO_AFRH_AFSEL8_Pos) | (4UL << GPIO_AFRH_AFSEL9_Pos);

	// Set PB8 and PB9 to high-speed, open-drain, pull-up
	GPIOB->OSPEEDR = (GPIOB->OSPEEDR
			& ~(GPIO_OSPEEDR_OSPEED8 | GPIO_OSPEEDR_OSPEED9))
			| (0b11UL << GPIO_OSPEEDR_OSPEED8_Pos)
			| (0b11UL << GPIO_OSPEEDR_OSPEED9_Pos);
	GPIOB->OTYPER |= GPIO_OTYPER_OT8 | GPIO_OTYPER_OT9;
	GPIOB->PUPDR = (GPIOB->PUPDR & ~(GPIO_PUPDR_PUPD8 | GPIO_PUPDR_PUPD9))
			| (0b01UL << GPIO_PUPDR_PUPD8_Pos)
			| (0b01UL << GPIO_PUPDR_PUPD9_Pos);

	// Select SYSCLK (72MHz) as I2C1 input
	RCC->CCIPR = (RCC->CCIPR & ~(RCC_CCIPR_I2C1SEL))
			| ((0b01 << RCC_CCIPR_I2C1SEL_Pos));

	// Enable I2C1 clock
	RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN;

	// Set TIMINGR for fast mode, 72MHz input clock
	I2C1->TIMINGR = 0x00E12573;

	// Enable I2C1
	I2C1->CR1 |= I2C_CR1_PE;

	// Set peripheral address to 0x3C, the LCD's address
	I2C1->CR2 = (I2C1->CR2 & ~(I2C_CR2_SADD))
			| (0x3CUL << (I2C_CR2_SADD_Pos + 1));

}
