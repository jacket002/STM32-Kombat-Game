// Jack Taylor
// 4/28/2026


/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "stm32l476xx.h"
#include<stdio.h>
#include<stdint.h>
#include "TIM2.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "mortalKombatLogo.h"
#include "GPIO.h"

uint32_t screenX_Center = 20;
uint32_t screenY_Center = 20;

volatile uint8_t input_detected_flag = 0;

uint32_t screenX_leftEdge = 0;
uint32_t screenX_rightEdge = 0;

uint32_t screenY_topEdge = 0;
uint32_t screenY_bottomEdge = 0;

// ============================
uint32_t player1_XCenter = 20;
uint32_t player1_YCenter = 20;

uint32_t player1_leftEdge = 0;
uint32_t player1_rightEdge = 0;

uint32_t player1_topEdge = 0;
uint32_t player1_bottomEdge = 0;
// ==============================

// =============================
uint32_t player2_XCenter = 107;
uint32_t player2_YCenter = 20;

uint32_t player2_leftEdge = 0;
uint32_t player2_rightEdge = 0;

uint32_t player2_topEdge = 0;
uint32_t player2_bottomEdge = 0;
//  =============================


/* 3. PLL Clock Configuration:
To generate the 80MHz clock, proceed as follows:
1. Set the Flash Wait States in FLASH_ACR.
2. Configure the High-Speed Internal Oscillator (HSI) in RCC_CR
3. Calibrate HSI16 in Internal clock sources calibration register (RCC_ICSCR)
4. Disable the PLL by setting PLLON to 0 in Clock control register (RCC_CR).
5. Wait until PLLRDY is cleared. The PLL is now fully stopped.
6. Change the desired parameter (PLLM, PLLN,PLLR) in RCC_PLLCFGR.
7. Select the PLL clock source in RCC->PLLCFGR.
8. Enable the PLL again by setting PLLON to 1 in RCC->PLLCR
9. Enable the desired PLL outputs by configuring PLLREN, PLLQEN, PLLREN in PLL
configuration register (RCC_PLLCFGR).
10. System Clock Selection: Select PLL selected as system clock by ensuring that
it is correctly selected in RCC->CFGR */

int main(void)
{

	System_Clock_Init(); // initialize system clock
	SysTick_Initialize(79); // 79 ticks needed for time period of 1 microsecond.
	BUZZER_PIN_INIT();
	TIM2_Init();
	TIM3_Init();
	I2C_Init();
	ssd1306_Init();
	//ADC_Init();
	controller_Init();

	ssd1306_Fill(Black);
	ssd1306_DrawBitmap(0, 0, mortalKombatLogo.data, mortalKombatLogo.width, mortalKombatLogo.height, White);
	ssd1306_UpdateScreen();

	for (int i = 0; i < (4*80000); i++);

	while (1)
	  {

		if (input_detected_flag == 0) {

			ssd1306_Fill(Black);
			ssd1306_DrawBitmap(0, 0, mortalKombatLogo.data, mortalKombatLogo.width, mortalKombatLogo.height, White);

			ssd1306_SetCursor(50, 40);
			ssd1306_WriteString("Mortal Kombat!",Font_6x8,White);

			ssd1306_UpdateScreen();

			for (int i = 0; i < (7*80000); i++);
			ssd1306_Fill(Black);

			ssd1306_SetCursor(50, 40);
			ssd1306_WriteString("Mortal Kombat!",Font_6x8,White);

			ssd1306_UpdateScreen();
			for (int i = 0; i < (7*80000); i++);

			if (!(GPIOA->IDR & GPIO_IDR_ID4) | !(GPIOA->IDR & GPIO_IDR_ID5)) {
				input_detected_flag = 1;
			}


		} else {

		// Clear display buffer
		    ssd1306_Fill(Black);

		    draw_Player1();
		    draw_Player2();

		    ssd1306_UpdateScreen();


		    // Add a tiny delay to pace the movement speed
		    for(volatile int i = 0; i < 20000; i++);

	  }
  /* USER CODE END 3 */
}

}

// Initialize I2C1 for communication with the LCD screen.
// Configuration is controller mode, fast mode, with the peripheral address set to 0x3C.
void I2C_Init(void) {
	// Enable clock to port B
		RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;

		// FIX: Read from GPIOB->MODER, not GPIOA->MODER!
		GPIOB->MODER = (GPIOB->MODER & ~(GPIO_MODER_MODE8 | GPIO_MODER_MODE9))
				| (0b10UL << GPIO_MODER_MODE8_Pos) | (0b10UL << GPIO_MODER_MODE9_Pos);

		// FIX: Read from GPIOB->AFR[1], not GPIOA->AFR[1]!
		GPIOB->AFR[1] = (GPIOB->AFR[1] & ~(GPIO_AFRH_AFSEL8 | GPIO_AFRH_AFSEL9))
				| (4UL << GPIO_AFRH_AFSEL8_Pos) | (4UL << GPIO_AFRH_AFSEL9_Pos);

		// Set PB8 and PB9 to high-speed, open-drain, pull-up
		GPIOB->OSPEEDR = (GPIOB->OSPEEDR & ~(GPIO_OSPEEDR_OSPEED8 | GPIO_OSPEEDR_OSPEED9))
				| (0b11UL << GPIO_OSPEEDR_OSPEED8_Pos) | (0b11UL << GPIO_OSPEEDR_OSPEED9_Pos);
		GPIOB->OTYPER |= GPIO_OTYPER_OT8 | GPIO_OTYPER_OT9;
		GPIOB->PUPDR = (GPIOB->PUPDR & ~(GPIO_PUPDR_PUPD8 | GPIO_PUPDR_PUPD9))
				| (0b01UL << GPIO_PUPDR_PUPD8_Pos) | (0b01UL << GPIO_PUPDR_PUPD9_Pos);

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

void draw_Player1() {


	// Read Input Data Register for PA2 (Left Button)
			    // If the bit is 0, the button is physically pressed down
			    if (!(GPIOA->IDR & GPIO_IDR_ID4)) {
			        // Move your player or pixel left
			    	player1_XCenter++;
			    	player1_leftEdge = (player1_XCenter - 5);
			    	player1_rightEdge = (player1_XCenter + 5);

			    }

			    if (!(GPIOA->IDR & GPIO_IDR_ID5)) {
			        // Move your player or pixel left
			    	player1_XCenter--;
			    	player1_rightEdge = (player1_XCenter + 5);
			        player1_leftEdge = (player1_XCenter - 5);

		        }

			    if (player1_leftEdge <= 1) {
			    	player1_leftEdge = 1;
			    	player1_XCenter = player1_leftEdge + 5;
			    	player1_rightEdge = player1_XCenter + 5;

			   	} else if (player1_rightEdge >= 127) {
			   		player1_rightEdge = 127;
			   		player1_XCenter = player1_rightEdge - 5;
			   		player1_leftEdge = player1_XCenter - 5;
			   	}


			    if (player1_rightEdge >= (player2_leftEdge - 2)) {

			    		player1_rightEdge = (player2_leftEdge - 2);
			    		player1_XCenter = player1_rightEdge - 5;
			    		player1_leftEdge = player1_XCenter - 5;


			    }


			    // Draw the current position frame and update screen
			    ssd1306_DrawRectangle(player1_leftEdge, (player1_YCenter-5), player1_rightEdge, (player1_YCenter+5), White);

}

void draw_Player2() {

	player2_leftEdge = (player2_XCenter - 5);
	player2_rightEdge = (player2_XCenter + 5);

	player2_topEdge = (player2_YCenter - 5);
	player2_bottomEdge = (player2_YCenter + 5);

	// Draw the current position frame and update screen
    ssd1306_DrawRectangle(player2_leftEdge, player2_topEdge, player2_rightEdge, player2_bottomEdge, White);


}


