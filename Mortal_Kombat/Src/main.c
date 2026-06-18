// Jack Taylor
// 4/28/2026

/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include <controller_GPIO.h>
#include "stm32l476xx.h"
#include<stdio.h>
#include<stdint.h>
#include <TIM.h>
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "mortalKombatLogo.h"
#include "display.h"
#include "player.h"
#include "physics.h"

volatile uint8_t input_detected_flag = 0;

extern volatile uint32_t secondsElapsed;

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

int main(void) {

	System_Clock_Init(); // initialize system clock
	SysTick_Initialize(79999); // 79999 ticks needed for time period of 1 millisecond.
	BUZZER_PIN_INIT();
	TIM2_Init();
	TIM3_Init();
	TIM15_Init();
	I2C_Init();
	ssd1306_Init();
	//ADC_Init();
	controller_Init();

	ssd1306_Fill(Black);
	ssd1306_DrawBitmap(0, 0, mortalKombatLogo.data, mortalKombatLogo.width,
			mortalKombatLogo.height, White);
	ssd1306_UpdateScreen();

	for (int i = 0; i < (4 * 80000); i++);

	while (1) {

		// While user input isn't detected, loop intro
		if (input_detected_flag == 0) {

			// If any button from controller 1 is pressed, set input detected flag
			if (!(GPIOA->IDR & GPIO_IDR_ID4) | !(GPIOA->IDR & GPIO_IDR_ID5)
					| !(GPIOA->IDR & GPIO_IDR_ID6)) {
				input_detected_flag = 1;

			}

			// Draw logo every 2 seconds
			if (secondsElapsed % 2 == 0) {

				ssd1306_Fill(Black);
				ssd1306_DrawBitmap(0, 0, mortalKombatLogo.data,
						mortalKombatLogo.width, mortalKombatLogo.height, White);

			} else {

				// blank screen otherwise
				ssd1306_Fill(Black);
			}

			// At 5< seconds, Draw Mortal Kombat title
			if (secondsElapsed > 5) {
				ssd1306_SetCursor(25, 40);
				ssd1306_WriteString("Mortal Kombat!", Font_6x8, White);
			}

			ssd1306_UpdateScreen();

		} else {

			game();

		}

	}

}
