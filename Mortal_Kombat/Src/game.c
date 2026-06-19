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
#include "game.h"

volatile char buf[20]; // buffer for characters

volatile uint8_t reset_match_flag = 1;
volatile uint8_t reset_time_flag = 0;

volatile uint32_t matchTime = 10;

extern volatile uint32_t secondsElapsed;

volatile uint32_t last_16ms = 0;

playerData player1;
playerData player2;

void game(void) {

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
					player_Jump(&player1);
				}

				update_PlayerPhysics(&player1, c1_R1_Active, c1_S1_Active,
						c1_L1_Active, c1_J1_Active);
				update_PlayerPhysics(&player2, 0, 0, 0, 0);

				resolve_Collisions(&player1, &player2);

				update_PlayerDirections(&player1, &player2);

				draw_Player(&player1);
				draw_Player(&player2);

				draw_HealthBar();
				draw_MatchTime();
				tally();
				//drawFloor();
				//drawCeiling();

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

// Resets match
void match_Reset(void) {

	reset_match_flag = 0;

	ssd1306_Fill(Black);
	draw_HealthBar();

	player_ResetGlobal();

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

void player_ResetGlobal() {

	player1.state = IDLE;
	player2.state = IDLE;

	player1.health = 100;
	player2.health = 100;

	// Physical Initializations

	player_Stage(&player1, &player2);  // Stage players in starting positions

	update_PlayerPhysics(&player1, c1_R1_Active, c1_S1_Active, c1_L1_Active, // quick refresh of physics
			c1_J1_Active);
	update_PlayerPhysics(&player2, 0, 0, 0, 0);

	update_PlayerDirections(&player1, &player2); // set & check player directions

	// Draw both players on screen
	draw_Player(&player1);
	draw_Player(&player2);
}

void drawFloor(void) {
	ssd1306_DrawRectangle(0, SCREEN_FLOOR - 2, 127, SCREEN_FLOOR + 2, White);
}

void drawCeiling(void) {
	ssd1306_DrawRectangle(0, SCREEN_CEILING - 2, 127, SCREEN_CEILING + 2, White);
}
