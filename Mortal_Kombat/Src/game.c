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
#include "DMA.h"

volatile char buf[36]; // buffer for characters

volatile uint8_t reset_match_flag = 1;
volatile uint8_t reset_time_flag = 0;

volatile uint32_t matchTime = 10;

extern volatile uint32_t secondsElapsed;

volatile uint32_t last_16ms = 0;

playerData player1;
playerData player2;

void game(void) {

	player1.ctrIndex = 0 * 2;
	player2.ctrIndex = 1 * 2;

	if (!(is_Match_Finished(&player1, &player2))) {

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
				uint8_t p1_strike = 0;
				uint8_t p2_strike = 0;

				if (player1.state == JUMPING || player2.state == JUMPING) {
					if (player1.state == JUMPING) {
						player_Jump(&player1);
					}
					if (player2.state == JUMPING) {
						player_Jump(&player2);
					}
				}

				// Call attack functions for respective player attacking action
				if (player1.actionState == ACT_ATTACKING
						|| player2.actionState == ACT_ATTACKING) {
					if (player1.actionState == ACT_ATTACKING) {
						p1_strike = player_Attack(&player1);
					}
					if (player2.actionState == ACT_ATTACKING) {
						player_Attack(&player2);
						p2_strike = player_Attack(&player2);

					}
				}

				update_PlayerPhysics(&player1, c1_S1_Active);
				//update_PlayerPhysics(&player2, 0);

				resolve_Collisions(&player1, &player2);

				update_PlayerDirections(&player1, &player2);

				match_Combat_State(&player1, &player2, p1_strike, p2_strike);

				update_PlayerHealth(&player1);
				update_PlayerHealth(&player2);

				draw_Player(&player1);
				draw_Player(&player2);

				draw_HealthBar(&player1, &player2);
				draw_MatchTime();
				tally();
				//drawFloor();
				//drawCeiling();

				ssd1306_UpdateScreen();

				last_16ms = millis();

			}

		} else {

			uint8_t player_Won = evaluate_Results(&player1, &player2);
			// Player with most health at the end of the match wins the round
			ssd1306_SetCursor(20, 30);
			if (player_Won == 1) {
				ssd1306_WriteString("Player 1 Wins!", Font_6x8, White);
			} else if (player_Won == 2) {
				ssd1306_WriteString("Player 2 Wins!", Font_6x8, White);
			} else if (player_Won == 0) {
				ssd1306_WriteString("Draw!", Font_6x8, White);
			}

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
void draw_HealthBar(playerData *p1, playerData *p2) {

	// Player 1 healthbar frame coordinates
	int8_t startX1 = 5;
	int8_t startY1 = 2;
	int8_t width1 = 50;
	int8_t height1 = 8;

	// Define real-time reamaining player health
	int8_t p1_remaining = (int) ((p1->health) * 0.50);
	if (p1_remaining <= startX1 + 2) {
		p1_remaining = startX1 + 3;
	}

	// Player 2 healthbar coordinates
	int8_t startX2 = 78;
	int8_t startY2 = 2;
	int8_t width2 = 123;
	int8_t height2 = 8;

	ssd1306_DrawRectangle(startX1, startY1, width1, height1, White); // player 1 healthbar prototype

	ssd1306_FillRectangle(startX1 + 2, startY1 + 2, p1_remaining - 2,
			height1 - 2, White); // health remaining

	//
	int8_t p2_remaining = (int) ((p2->health) * 0.45);
	// if remaining player health relative to healthbar dimensions is less than or equal to 0, clamp
	if (p2_remaining <= 0) {
		p2_remaining = 0;
	}

	int8_t p2_fill_remaining_x_start = startX2 + 2;
	int8_t p2_fill_remaining_x_pos = p2_fill_remaining_x_start + p2_remaining; // defines the remaining health x-coord
	int8_t p2_fill_remaining_x_end = p2_fill_remaining_x_pos - 2;

	if (p2_fill_remaining_x_end <= p2_fill_remaining_x_start) {
		p2_fill_remaining_x_end = p2_fill_remaining_x_start;
	}

	ssd1306_DrawRectangle(startX2, startY2, width2, height2, White); // player 2 healthbar prototype

	if (p2_fill_remaining_x_end > p2_fill_remaining_x_start) {
		ssd1306_FillRectangle(p2_fill_remaining_x_start, startY2 + 2,
				p2_fill_remaining_x_end, height2 - 2, White); // health remaining
	}
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
	draw_HealthBar(&player1, &player2);

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

	if (player1.roundsWon >= 1) {
		ssd1306_FillRectangle(x1_Tally1, y1_Tally1, x2_Tally1, y2_Tally1,
				White); // Player 1 round tally prototype
	}

	if (player1.roundsWon == 2) {
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

	update_PlayerPhysics(&player1, c1_S1_Active);
	//update_PlayerPhysics(&player2, 0);

	update_PlayerDirections(&player1, &player2); // set & check player directions

	// Draw both players on screen
	draw_Player(&player1);
	draw_Player(&player2);
}

void drawFloor(void) {
	ssd1306_DrawRectangle(0, SCREEN_FLOOR - 2, 127, SCREEN_FLOOR + 2, White);
}

void drawCeiling(void) {
	ssd1306_DrawRectangle(0, SCREEN_CEILING - 2, 127, SCREEN_CEILING + 2,
			White);
}

