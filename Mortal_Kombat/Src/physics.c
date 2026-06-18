#include "stm32l476xx.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "physics.h"
#include "player.h"

// ============================
uint32_t player1_XCenter = 20;
uint32_t player1_YCenter = 50;

volatile uint32_t player1_YVelocity = -6;
// ==============================

// =============================
uint32_t player2_XCenter = 65;
uint32_t player2_YCenter = 50;
//  =============================

// Evaluates input from controller 1 and moves player1 sprite accordingly
void update_PlayerPhysics(playerData *player, uint8_t R1_Active, uint8_t S1_Active, uint8_t L1_Active, uint8_t J1_Active) {

	// Read Input Data Register for PA2 (Left Button)
	// If the bit is 0, the button is physically pressed down
	if (R1_Active) {
		// Move player to the right
		player->xPos += 2;
		player->leftEdge = (player->xPos - 5);
		player->rightEdge = (player->xPos + 5);

	}


	if (S1_Active) {

		if (!(player->state == ATTACKING)) {
			player->state = ATTACKING;
		}

	}


	// If button L1 is pressed
	if (L1_Active) {

		// Move player to the left
		player->xPos -= 2;
		player->rightEdge = (player->xPos + 5);
		player->leftEdge = (player->xPos - 5);

	}

	// If jump button (middle) is pressed
	if (J1_Active) {

		if (player->state != JUMPING) {
			player->yVel = -9;
			player->state = JUMPING;
		}

		/*
		 player1_YCenter = (player1_YCenter - 5);
		 player1_topEdge = (player1_YCenter - 5);
		 player1_bottomEdge = (player1_YCenter + 5);
		 */

	} else {

		if (player->state != JUMPING) {
			player->yPos = (player->yPos);
			player->topEdge = (player->yPos - 5);
			player->bottomEdge = (player->yPos + 5);

		}

	}

	if (player->topEdge <= SCREEN_CEILING) {

		player->topEdge = SCREEN_CEILING;
		player->yPos = player->topEdge + 5;
		player->bottomEdge = player->yPos + 5;

	}

	if (player->leftEdge <= 1) {
		player->leftEdge = 1;
		player->xPos = player->leftEdge + 5;
		player->rightEdge = player->xPos + 5;

	} else if (player->rightEdge >= 127) {
		player->rightEdge = 127;
		player->xPos = player->rightEdge - 5;
		player->leftEdge = player->xPos - 5;
	}


}

/*
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
 */

// Set player1 to starting match position
void player_Stage(playerData *p1, playerData *p2) {

	p1->xPos = PLAYER1_STARTING_X_POS;
	p1->rightEdge = p1->xPos + 5;
	p1->leftEdge = p1->xPos - 5;

	p1->yPos = PLAYER1_STARTING_Y_POS;
	p1->topEdge = p1->yPos - 5;
	p1->bottomEdge = p1->yPos + 5;

	p2->xPos = PLAYER2_STARTING_X_POS;
	p2->rightEdge = p2->xPos + 5;
	p2->leftEdge = p2->xPos - 5;

	p2->yPos = PLAYER2_STARTING_Y_POS;
	p2->topEdge = p2->yPos - 5;
	p2->bottomEdge = p2->yPos + 5;

}

// Evaluate player1 jumping state and physics
void player_Jump(playerData *p) {

	// If jumping state is 1
	if (p->state == JUMPING) {

		p->yPos += p->yVel;

		p->yVel += 1;

		p->topEdge = (p->yPos - 5);
		p->bottomEdge = (p->yPos + 5);

		if (p->bottomEdge >= SCREEN_FLOOR) {
			p->state = IDLE;

			p->bottomEdge = SCREEN_FLOOR;
			p->yPos = (p->bottomEdge - 5);
			p->topEdge = (p->yPos - 5);
			p->yVel = 0;
		}

	}

}


/*
// Evaluate player1 jumping state and physics
void player_Attack(playerData *player) {

	// If jumping state is 1
	if (player->state == ATTACKING) {

	}

}
*/

void resolve_Collisions(playerData *p1, playerData *p2) {

	if ((p1->rightEdge) >= (p2->leftEdge - 2)
			&& (p1->leftEdge) <= (p2->rightEdge + 2)) {

		if ((p1->bottomEdge) >= (p2->topEdge - 2)) {

			if (p1->xPos >= (p2->xPos)) {

				p1->leftEdge = (p2->rightEdge + 2);
				p1->xPos = p1->leftEdge + 5;
				p1->rightEdge = p1->xPos + 5;

			} else {
				p1->rightEdge = (p2->leftEdge - 2);
				p1->xPos = p1->rightEdge - 5;
				p1->leftEdge = p1->xPos - 5;
			}

		}

	}

}

void draw_Player(playerData *player) {

	// Draw the current position frame and update screen
		ssd1306_DrawRectangle(player->leftEdge, player->topEdge, player->rightEdge,
				player->bottomEdge, White);

}
