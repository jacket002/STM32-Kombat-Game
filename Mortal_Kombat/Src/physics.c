#include "stm32l476xx.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "physics.h"
#include "player.h"
#include "sprites.h"

int32_t hWidth = 0;
int32_t hHeight = 0;

// Evaluates input from controller 1 and moves player1 sprite accordingly
void update_PlayerPhysics(playerData *player, uint8_t R1_Active,
		uint8_t S1_Active, uint8_t L1_Active, uint8_t J1_Active) {

	int32_t hWidth = (player->character.boxWidth / 2);
	int32_t hHeight = (player->character.boxHeight / 2);

	// Read Input Data Register for PA2 (Left Button)
	// If the bit is 0, the button is physically pressed down
	if (R1_Active) {
		// Move player to the right
		player->xPos += 2;

		player->leftEdge = (player->xPos - hWidth);
		player->rightEdge = (player->xPos + hWidth);

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
		player->rightEdge = (player->xPos + hWidth);
		player->leftEdge = (player->xPos - hWidth);

	}

	// If jump button (middle) is pressed
	if (J1_Active) {

		if (player->state != JUMPING) {
			player->yVel = -9;
			player->state = JUMPING;
		}

	} else {

		if (player->state != JUMPING) {
			player->yPos = (player->yPos);
			player->topEdge = (player->yPos - hHeight);
			player->bottomEdge = (player->yPos + hHeight);

		}

	}

	if (player->topEdge <= (SCREEN_CEILING)) {

		player->topEdge = (SCREEN_CEILING);
		player->yPos = player->topEdge + hHeight;
		player->bottomEdge = player->yPos + hHeight;

	}

	if (player->leftEdge <= 1) {
		player->leftEdge = 1;
		player->xPos = player->leftEdge + hWidth;
		player->rightEdge = player->xPos + hWidth;

	} else if (player->rightEdge >= 127) {
		player->rightEdge = 127;
		player->xPos = player->rightEdge - hWidth;
		player->leftEdge = player->xPos - hWidth;
	}

}

// Set player1 to starting match position
void player_Stage(playerData *p1, playerData *p2) {

	// Set player direction, hard set sprite according to dir
	p1->playerDir = 'r';
	p1->character.currentSprite = IDLE_TEST_R;

	// Hard set hit box dimensions to 75% of sprite dimensions
	p1->character.boxWidth =
			(int32_t) (p1->character.currentSprite.width * 0.50);
	p1->character.boxHeight = (int32_t) (p1->character.currentSprite.height
			* 0.75);

	hWidth = p1->character.boxWidth / 2;
	hHeight = p1->character.boxHeight / 2;

	// Initialize player coordinates
	p1->xPos = PLAYER1_STARTING_X_POS;
	p1->bottomEdge = SCREEN_FLOOR;
	p1->yPos = p1->bottomEdge - hHeight;
	p1->topEdge = p1->bottomEdge  - p1->character.boxHeight;
	p1->leftEdge = p1->xPos - hWidth;
	p1->rightEdge = p1->xPos + hWidth;

	p2->playerDir = 'l';
	p2->character.currentSprite = IDLE_TEST_L;

	// Hard set hit box dimensions to 75% of sprite dimensions
	p2->character.boxWidth =
			(int32_t) (p2->character.currentSprite.width * 0.50);
	p2->character.boxHeight = (int32_t) (p2->character.currentSprite.height
			* 0.75);

	hWidth = p2->character.boxWidth / 2;
	hHeight = p2->character.boxHeight / 2;

	// Initialize player coordinates
	p2->xPos = PLAYER2_STARTING_X_POS;
	p2->bottomEdge = SCREEN_FLOOR;
	p2->yPos = p2->bottomEdge - hHeight;
	p2->topEdge = p2->bottomEdge  - p2->character.boxHeight;
	p2->leftEdge = p2->xPos - hWidth;
	p2->rightEdge = p2->xPos + hWidth;

}

// Evaluate player1 jumping state and physics
void player_Jump(playerData *p) {

	hWidth = p->character.boxWidth / 2;
	hHeight = p->character.boxHeight / 2;

	int32_t halfhalfWidth = hWidth / 2;
	int32_t halfhalfHeight = hHeight / 2;

	// If jumping state is 1
	if (p->state == JUMPING) {

		p->yPos += p->yVel;

		p->yVel += 1;

		p->topEdge = (p->yPos - halfhalfHeight);
		p->bottomEdge = (p->yPos + halfhalfHeight);

		if (p->bottomEdge >= (SCREEN_FLOOR)) {
			p->state = IDLE;

			p->bottomEdge = (SCREEN_FLOOR);
			p->yPos = p->bottomEdge - hHeight;
			p->topEdge = p->yPos - hHeight;
			p->yVel = 0;
		}

	}

}

// Evaluate player1 jumping state and physics
void player_Attack(playerData *player) {

	// If jumping state is 1
	if (player->state == ATTACKING) {

	}

}

void resolve_Collisions(playerData *p1, playerData *p2) {

	int32_t p1_hWidth = (p1->character.boxWidth / 2);

	if ((p1->rightEdge) >= (p2->leftEdge - 2)
			&& (p1->leftEdge) <= (p2->rightEdge + 2)) {

		if ((p1->bottomEdge) >= (p2->topEdge - 2)) {

			if (p1->xPos >= (p2->xPos)) {

				// Push player 1 to the right of player 2's box
				p1->leftEdge = (p2->rightEdge + 2);
				p1->xPos = p1->leftEdge + p1_hWidth;
				p1->rightEdge = p1->xPos + p1_hWidth;

			} else {
				p1->rightEdge = (p2->leftEdge - 2);
				p1->xPos = p1->rightEdge - p1_hWidth;
				p1->leftEdge = p1->xPos - p1_hWidth;
			}

		}

	}

}

void update_PlayerDirections(playerData *p1, playerData *p2) {

	if (p1->xPos < p2->xPos) {
		p1->playerDir = 'r';
		p2->playerDir = 'l';

	}

	else if (p1->xPos > p2->xPos) {
		p1->playerDir = 'l';
		p2->playerDir = 'r';
	}
}

void draw_Player(playerData *player) {

	uint8_t xOff = player->character.currentSprite.offsetX;
	uint8_t yOff = player->character.currentSprite.offsetY;

	if (player->state == IDLE) {

		if (player->playerDir == 'r') {
			player->character.currentSprite = IDLE_TEST_R;

		} else if (player->playerDir == 'l') {
			player->character.currentSprite = IDLE_TEST_L;

		}

		player->character.currentSprite.offsetX = -10;
		player->character.currentSprite.offsetY = -13;

	}

	// Sprite view
	ssd1306_DrawBitmap(player->xPos + xOff, player->yPos + yOff,
			player->character.currentSprite.data,
			player->character.currentSprite.width,
			player->character.currentSprite.height, White);
	// Hitbox view
	ssd1306_DrawRectangle(player->leftEdge, player->topEdge, player->rightEdge,
			player->bottomEdge, White);

	/*
	 // Draw the current position frame and update screen
	 ssd1306_DrawRectangle(player->leftEdge, player->topEdge, player->rightEdge,
	 player->bottomEdge, White);
	 */
}

void inject_HitBox_Data(playerData *player) {

}

