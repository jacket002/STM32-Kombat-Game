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

// ============================
uint32_t player1_XCenter = 20;
uint32_t player1_YCenter = 50;

uint32_t player1_leftEdge = 0;
uint32_t player1_rightEdge = 0;

uint32_t player1_topEdge = 0;
uint32_t player1_bottomEdge = 0;

volatile uint32_t player1_YVelocity = -6;
// ==============================

// =============================
uint32_t player2_XCenter = 107;
uint32_t player2_YCenter = 50;

uint32_t player2_leftEdge = 0;
uint32_t player2_rightEdge = 0;

uint32_t player2_topEdge = 0;
uint32_t player2_bottomEdge = 0;
//  =============================

volatile char buf[20]; // buffer for characters

volatile uint8_t input_detected_flag = 0;
volatile uint8_t reset_match_flag = 0;
volatile uint8_t reset_time_flag = 0;

volatile uint32_t matchTime = 10;

volatile uint8_t currentRound = 0;
uint8_t maxRounds = 3;

extern volatile uint32_t secondsElapsed;
volatile uint32_t last_16ms = 0;

#define c1_L1_Active !(GPIOA->IDR & GPIO_IDR_ID5)
#define c1_R1_Active !(GPIOA->IDR & GPIO_IDR_ID4)
#define c1_J1_Active !(GPIOA->IDR & GPIO_IDR_ID6)

#define SCREEN_FLOOR 55
#define SCREEN_CEILING 2
#define SCREEN_CENTER_X 64 // 128div2
#define SCREEN_CENTER_Y 32 // 64div2

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

playerData player1;
playerData player2;

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

	for (int i = 0; i < (4 * 80000); i++)
		;

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

			if (!(player1.wins >= 2)) {

				// If match reset request is queued, reset & redraw the match
				if (reset_match_flag == 1) {
					match_Reset();
				}

				// If the total match time remaining is greater than 0, loop the main game
				if (matchTime - secondsElapsed > 0) {

					if (reset_time_flag == 0) {
						secondsElapsed = 0; // reset TIM15 count
						matchTime = 10; // reset match time
						reset_time_flag = 1;
					}

					// Execute everything at exactly 60 Hz
					if (millis() - last_16ms >= 16) {

						ssd1306_Fill(Black);

						if (player1.state == JUMPING) {
							player1_Jump();
						}

						draw_Player1();
						draw_Player2();
						draw_HealthBar();
						draw_MatchTime();
						tally();

						ssd1306_UpdateScreen();

						last_16ms = millis();

					}

				} else {

					player1.wins += 1;

					// Player with most health at the end of the match wins the round
					ssd1306_SetCursor(20, 30);
					ssd1306_WriteString("Player 1 Wins!", Font_6x8, White);
					draw_MatchTime();
					tally();

					ssd1306_UpdateScreen();

					// If the reset time request flag is set, reset the secondsElapsed and enable reset match request
					if (reset_time_flag == 1) {
						secondsElapsed = 0;
						reset_time_flag = 0;
						reset_match_flag = 1;
					}

					// 2 second delay before next match
					while (!(secondsElapsed == 2))
						;

					// Reset secondsElapsed and match time
					secondsElapsed = 0;
					matchTime = 99;

				}

			} else {

				// Pass player data by reference, clear the entire data stack for the structs
				playerData_Init(&player1);
				playerData_Init(&player2);

				ssd1306_Fill(Black);
				ssd1306_SetCursor(20, 30);

				ssd1306_WriteString("Game over", Font_6x8, White);
				ssd1306_UpdateScreen();

			}

		}

	}

}

// Evaluates input from controller 1 and moves player1 sprite accordingly
void draw_Player1(void) {

	// Read Input Data Register for PA2 (Left Button)
	// If the bit is 0, the button is physically pressed down
	if (c1_R1_Active) {
		// Move player to the right
		player1_XCenter += 2;
		player1_leftEdge = (player1_XCenter - 5);
		player1_rightEdge = (player1_XCenter + 5);

	}

	// If button L1 is pressed
	if (c1_L1_Active) {

		// Move player to the left
		player1_XCenter -= 2;
		player1_rightEdge = (player1_XCenter + 5);
		player1_leftEdge = (player1_XCenter - 5);

	}

	// If jump button (middle) is pressed
	if (c1_J1_Active) {

		if (!(player1.state == JUMPING)) {
			player1_YVelocity = -8;
			player1.state = JUMPING;
		}

		/*
		 player1_YCenter = (player1_YCenter - 5);
		 player1_topEdge = (player1_YCenter - 5);
		 player1_bottomEdge = (player1_YCenter + 5);
		 */

	} else {

		player1_YCenter = (player1_YCenter);
		player1_topEdge = (player1_YCenter - 5);
		player1_bottomEdge = (player1_YCenter + 5);

	}

	if (player1_topEdge <= SCREEN_CEILING) {

		player1_topEdge = SCREEN_CEILING;
		player1_YCenter = player1_topEdge + 5;
		player1_bottomEdge = player1_YCenter + 5;

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

	if ((player1_rightEdge) >= (player2_leftEdge - 2)) {

		if ((player1_bottomEdge) >= (player2_topEdge - 2)) {
			player1_rightEdge = (player2_leftEdge - 2);
			player1_XCenter = player1_rightEdge - 5;
			player1_leftEdge = player1_XCenter - 5;
		}

	}

	// Draw the current position frame and update screen
	ssd1306_DrawRectangle(player1_leftEdge, player1_topEdge, player1_rightEdge, player1_bottomEdge, White);

}

// Draw player 2 (static)
void draw_Player2(void) {

	player2_leftEdge = (player2_XCenter - 5);
	player2_rightEdge = (player2_XCenter + 5);

	player2_topEdge = (player2_YCenter - 5);
	player2_bottomEdge = (player2_YCenter + 5);

	// Draw the current position frame and update screen
	ssd1306_DrawRectangle(player2_leftEdge, player2_topEdge, player2_rightEdge,
			player2_bottomEdge, White);

}

// Draw both health bars (static)
void draw_HealthBar(void) {

	// Player 1 healthbar coordinates
	uint8_t startX1 = 5;
	uint8_t startY1 = 2;
	uint8_t width1 = 50;
	uint8_t height1 = 8;

	// Player 2 healthbar coordinates
	uint8_t startX2 = 78;
	uint8_t startY2 = 2;
	uint8_t width2 = 123;
	uint8_t height2 = 8;

	ssd1306_DrawRectangle(startX1, startY1, width1, height1, White); // player 1 healthbar prototype

	ssd1306_FillRectangle(startX1 + 2, startY1 + 2, width1 - 2, height1 - 2,
			White); // health remaining

	ssd1306_DrawRectangle(startX2, startY2, width2, height2, White); // player 2 healthbar prototype
	ssd1306_FillRectangle(startX2 + 2, startY2 + 2, width2 - 2, height2 - 2,
			White); // health remaining

}

// Draw match time instance w/ concurrent seconds elapsed
void draw_MatchTime(void) {

	ssd1306_SetCursor(60, 3);

	// remaining time = difference of match time from seconds elapsed
	int32_t remaining = matchTime - secondsElapsed;
	// Sets ground value for match time to 0 (no negative time)
	if (remaining < 0) {
		remaining = 0;
	}

	// Write match time remaining to char buffer and print
	sprintf(buf, "%d", remaining);
	ssd1306_WriteString(buf, Font_6x8, White);
	;

}

// Set player1 to starting match position
void fix_Sprite1(void) {

	player1_XCenter = 20;

	player1_rightEdge = player1_XCenter + 5;
	player1_leftEdge = player1_XCenter - 5;

	player1_YCenter = 50;
	player1_topEdge = player1_YCenter - 5;
	player1_bottomEdge = player1_YCenter + 5;

}

// Evaluate player1 jumping state and physics
void player1_Jump(void) {

	// If jumping state is 1
	if (player1.state == JUMPING) {

		player1_YCenter += player1_YVelocity;

		player1_YVelocity += 1;

		player1_topEdge = (player1_YCenter - 5);
		player1_bottomEdge = (player1_YCenter + 5);

		if (player1_bottomEdge >= SCREEN_FLOOR) {
			player1.state = IDLE;

			player1_bottomEdge = SCREEN_FLOOR;
			player1_YCenter = (player1_bottomEdge - 5);
			player1_topEdge = (player1_YCenter - 5);
			player1_YVelocity = 0;
		}

	}

}

// Resets match
void match_Reset(void) {

	ssd1306_Fill(Black);
	draw_HealthBar();

	player1.state = IDLE;

	fix_Sprite1();

	reset_match_flag = 0;

	draw_Player1();
	draw_Player2();
	draw_MatchTime();

	ssd1306_UpdateScreen();

}

void tally(void) {

	uint8_t x1_Tally1 = 8;
	uint8_t y1_Tally1 = 13;
	uint8_t x2_Tally1 = 12;
	uint8_t y2_Tally1 = 16;

	uint8_t x1_Tally2 = 16;
	uint8_t y1_Tally2 = 13;
	uint8_t x2_Tally2 = 20;
	uint8_t y2_Tally2 = 16;

	if (player1.wins >= 1) {
		ssd1306_FillRectangle(x1_Tally1, y1_Tally1, x2_Tally1, y2_Tally1,
				White); // Player 1 round tally prototype
	}

	if (player1.wins == 2) {
		ssd1306_FillRectangle(x1_Tally2, y1_Tally2, x2_Tally2, y2_Tally2,
				White); // Player 1 round tally prototype
	}

}
