#include "stm32l476xx.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "physics.h"
#include "player.h"
#include "sprites.h"
#include "DMA.h"

int32_t player_hWidth = 0;
int32_t player_hHeight = 0;

int32_t pushBox_hWidth = 0;
int32_t pushBox_hHeight = 0;

// Evaluates input from controller 1 and moves player1 sprite accordingly
void update_PlayerPhysics(playerData *player, uint8_t S1_Active) {

	int32_t hWidth = (player->character.boxWidth / 2);
	int32_t hHeight = (player->character.boxHeight / 2);

	uint32_t X_voltage = joystick_data[player->ctrIndex];
	uint32_t Y_voltage = joystick_data[player->ctrIndex + 1];

	// If player is in jumping state, set joystick_data as if it's IDLE to block inputs during jump
	if (player->state == JUMPING) {
		X_voltage = 2072;
		Y_voltage = 2072;
	}

	// If player is in crouching state, lock joystick_data for x-input (strafing)
	if (player->state == CROUCHING) {
		X_voltage = 2072;
	}

	// Read Input Data Register for PA2 (Left Button)
	// If the bit is 0, the button is physically pressed down
	if (player->actionState != ACT_ATTACKING) {

		/*
		 * *Voltage thresholds were selected using a multimeter to analyze the voltage value\
		 *  at a reasonable angle for the joystick to trigger movement in software
		 *
		 *   V_in = baseline joystick voltage = 1.67 V
		 *
		 *  ADC Reading = (V_in / V_ref) * (2^N - 1)
		 *              = (1.67 / 3.3) * 4095 = 2072 (ADC)
		 *
		 *
		 * */

		if (X_voltage > 3700) {
			// Move player to the right
			player->xPos += 2;
			player->pushBox.xPos += 2;

			player->leftEdge = (player->xPos - hWidth);
			player->rightEdge = (player->xPos + hWidth);

			player->pushBox.l_Edge = player->xPos - 2;
			player->pushBox.r_Edge = player->xPos + 2;

		}

		// Set player state to attacking if strike button is active
		if (S1_Active) {

			if (player->buttonReleased == 1) {
				if (!(player->actionState == ACT_ATTACKING)) {
					player->actionState = ACT_ATTACKING;
				}
			}

			player->buttonReleased = 0;

		} else {
			player->buttonReleased = 1;
		}

		// If button L1 is pressed move character to the left
		if (X_voltage < 395) {

			// Move player to the left
			player->xPos -= 2;
			player->pushBox.xPos -= 2;

			player->rightEdge = (player->xPos + hWidth);
			player->leftEdge = (player->xPos - hWidth);

			player->pushBox.l_Edge = player->xPos - 2;
			player->pushBox.r_Edge = player->xPos + 2;

		}

		// If jump button (middle) is pressed, set player state to JUMPING and set player y velocity
		if (Y_voltage < 395) {

			if (player->state != JUMPING) {
				player->yVel = -9;

				if (X_voltage > 3700) {
					player->xVel = 2;
				}

				if (X_voltage < 395) {
					player->xVel = -2;
				}

				player->state = JUMPING;

			}

		} else {

			if (player->state != JUMPING) {

				player->yPos = (player->yPos);
				player->topEdge = (player->yPos - hHeight);
				player->bottomEdge = (player->yPos + hHeight);

				player->pushBox.yPos = player->pushBox.yPos;
				player->pushBox.t_Edge = (player->yPos - hHeight);
				player->pushBox.b_Edge = player->bottomEdge;

			}

		}

		// If joystick voltage reaches >3900 (down), player is crouching
		if (Y_voltage > 3900) {

			// Define crouching action if player is not jumping
			if (player->state != JUMPING) {
				player->state = CROUCHING;

				player->character.boxHeight = // Set the character's hurtbox height to half of the current sprites height
						(int32_t) (player->character.currentSprite.height * 0.50);
				player->bottomEdge = SCREEN_FLOOR; // Lock players bottom edge to the floor
				player->yPos = player->bottomEdge - hHeight; // Recenter y position to the new crouching height
				player->pushBox.b_Edge = player->bottomEdge; // lock pusbox to the bottom edge (screen floor)
			}
		} else {
			if (player->state != JUMPING) {
				player->state = IDLE;
				player->character.boxHeight =
						(int32_t) (player->character.currentSprite.height * 0.75);
				player->bottomEdge = SCREEN_FLOOR;
				player->yPos = player->bottomEdge - hHeight;
				player->pushBox.b_Edge = player->bottomEdge;

			}
		}
	}

	// Set player screen bound to the screen ceiling if the top edge is equal or greater to
	if (player->topEdge <= (SCREEN_CEILING)) {

		player->topEdge = (SCREEN_CEILING);
		player->yPos = player->topEdge + hHeight;
		player->bottomEdge = player->yPos + hHeight;

		player->pushBox.t_Edge = (SCREEN_CEILING);
		player->pushBox.yPos = player->yPos;
		player->pushBox.b_Edge = player->bottomEdge;

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

	// Hard set hurt box dimensions to 75% of sprite dimensions
	p1->character.boxWidth =
			(int32_t) (p1->character.currentSprite.width * 0.50);
	p1->character.boxHeight = (int32_t) (p1->character.currentSprite.height
			* 0.75);

	player_hWidth = p1->character.boxWidth / 2;
	player_hHeight = p1->character.boxHeight / 2;

	pushBox_hWidth = 2;
	pushBox_hHeight = 2;

	// Initialize player coordinates and hurtbox
	p1->xPos = PLAYER1_STARTING_X_POS;
	p1->bottomEdge = SCREEN_FLOOR;
	p1->yPos = p1->bottomEdge - player_hHeight;
	p1->topEdge = p1->bottomEdge - p1->character.boxHeight;
	p1->leftEdge = p1->xPos - player_hWidth;
	p1->rightEdge = p1->xPos + player_hWidth;

	p2->playerDir = 'l';
	p2->character.currentSprite = IDLE_TEST_L;

	// Initialize player pushbox
	p1->pushBox.xPos = p1->xPos;
	p1->pushBox.yPos = p1->yPos;
	p1->pushBox.l_Edge = p1->xPos - pushBox_hWidth;
	p1->pushBox.r_Edge = p1->xPos + pushBox_hWidth;
	p1->pushBox.b_Edge = p1->bottomEdge;
	p1->pushBox.t_Edge = p1->topEdge - 2;

	// Hard set hit box dimensions to 75% of sprite dimensions
	p2->character.boxWidth =
			(int32_t) (p2->character.currentSprite.width * 0.50);
	p2->character.boxHeight = (int32_t) (p2->character.currentSprite.height
			* 0.75);

	player_hWidth = p2->character.boxWidth / 2;
	player_hHeight = p2->character.boxHeight / 2;

	// Initialize player coordinates
	p2->xPos = PLAYER2_STARTING_X_POS;
	p2->bottomEdge = SCREEN_FLOOR;
	p2->yPos = p2->bottomEdge - player_hHeight;
	p2->topEdge = p2->bottomEdge - p2->character.boxHeight;
	p2->leftEdge = p2->xPos - player_hWidth;
	p2->rightEdge = p2->xPos + player_hWidth;

	// Initialize player pushbox
	p2->pushBox.xPos = p2->xPos;
	p2->pushBox.yPos = p2->yPos;
	p2->pushBox.l_Edge = p2->xPos - 2;
	p2->pushBox.r_Edge = p2->xPos + 2;
	p2->pushBox.b_Edge = p2->bottomEdge;
	p2->pushBox.t_Edge = p2->topEdge - 2;

}

// Evaluate player1 jumping state and physics
void player_Jump(playerData *p) {

	player_hWidth = p->character.boxWidth / 2;
	player_hHeight = p->character.boxHeight / 2;

	int32_t halfhalfWidth = player_hWidth / 2;
	int32_t halfhalfHeight = player_hHeight / 2;

	// If jumping state is 1
	if (p->state == JUMPING) {

		p->yPos += p->yVel;
		p->pushBox.yPos = p->yPos;

		p->xPos += p->xVel;
		p->pushBox.xPos = p->xPos;

		p->topEdge = (p->yPos - halfhalfHeight);
		p->bottomEdge = (p->yPos + halfhalfHeight);
		p->leftEdge = (p->xPos - player_hWidth);
		p->rightEdge = (p->xPos + player_hWidth);

		p->pushBox.t_Edge = p->topEdge - 2;
		p->pushBox.b_Edge = p->bottomEdge + 2;
		p->pushBox.l_Edge = p->xPos - 2;
		p->pushBox.r_Edge = p->xPos + 2;

		p->yVel += 1;

		if (p->bottomEdge >= (SCREEN_FLOOR)) {
			p->state = IDLE;

			p->bottomEdge = (SCREEN_FLOOR);
			p->yPos = p->bottomEdge - player_hHeight;
			p->topEdge = p->yPos - player_hHeight;
			p->yVel = 0;
			p->xVel = 0;

			p->pushBox.b_Edge = (SCREEN_FLOOR);
			p->pushBox.yPos = p->yPos;
			p->pushBox.t_Edge = p->topEdge - 2;

		}

	}

}

// Evaluate player1 jumping state and physics
int player_Attack(playerData *player) {

	static uint8_t current_frame = 0; // define static current_frame counter

	uint8_t totalFrames = character_Read_Frame_Data(player); // defines total frames as the array size of the frame data
	uint8_t strike_frame = 0;

	// If attacking state is set
	if (player->actionState == ACT_ATTACKING) {
		// reset current frame to 0 and set player state to IDLE when current_frame reaches total frame count
		if (current_frame >= totalFrames) {
			current_frame = 0;

			player->actionState = ACT_NONE;
		}

		// when current frame reaches midpoint of the total frame count
		if (current_frame == (totalFrames / 2)) {
			strike_frame = 1; // defines the frame where damage system is applied
		}
		// inject the hitbox data for each frame
		inject_HitBox_Data(player, current_frame);
		current_frame++;

	}
	return strike_frame;

}

void resolve_Collisions(playerData *p1, playerData *p2) {

	// Sets player1 half width
	int32_t p1_hWidth = (p1->character.boxWidth / 2);
	// Sets player1 push box half width
	int32_t p1_pushBox_hWidth = 2;

	int32_t p2_hWidth = (p2->character.boxWidth / 2);

	// Evaluates if player 1 right edge crosses player 2's left edge while player 1 hasnt fully overtaken player 2
	if ((p1->pushBox.r_Edge) >= (p2->pushBox.l_Edge - 1)
			&& (p1->pushBox.l_Edge) <= (p2->pushBox.r_Edge + 1)) {

		// If player 1 is landing in player 2s box, move player 1s box to the side of player 2 depending on the x-position
		if ((p1->pushBox.b_Edge) >= (p2->pushBox.t_Edge - 1)) {

			if (p1->pushBox.xPos >= (p2->pushBox.xPos)) {

				p1->pushBox.l_Edge = (p2->pushBox.r_Edge + 1);
				p1->pushBox.xPos = p1->pushBox.l_Edge + p1_pushBox_hWidth;
				p1->pushBox.r_Edge = p1->pushBox.xPos + p1_pushBox_hWidth;

				// Push player 1 to the right of player 2's box
				p1->xPos = p1->pushBox.xPos;
				p1->leftEdge = p1->xPos - p1_hWidth;
				p1->rightEdge = p1->xPos + p1_hWidth;

			} else {

				p1->pushBox.r_Edge = (p2->pushBox.l_Edge - 1);
				p1->pushBox.xPos = p1->pushBox.r_Edge - p1_pushBox_hWidth;
				p1->pushBox.l_Edge = p1->pushBox.xPos - p1_pushBox_hWidth;

				p1->xPos = p1->pushBox.xPos;
				p1->leftEdge = p1->xPos - p1_hWidth;
				p1->rightEdge = p1->xPos + p1_hWidth;

			}

		}

		if (p1->state != JUMPING && p2->state != JUMPING) {

			if (p1->playerDir == 'r') {

				if (p1->pushBox.r_Edge >= p2->pushBox.l_Edge - 1) {

					p2->xPos += 1;

					p2->pushBox.xPos += 1;

					p2->leftEdge = (p2->xPos - p2_hWidth);
					p2->rightEdge = (p2->xPos + p2_hWidth);

					p2->pushBox.l_Edge = p2->xPos - 2;
					p2->pushBox.r_Edge = p2->xPos + 2;
				}

			} else {

				if (p1->pushBox.l_Edge <= p2->pushBox.r_Edge + 1) {

					p2->xPos -= 1;

					p2->pushBox.xPos -= 1;

					p2->leftEdge = (p2->xPos - p2_hWidth);
					p2->rightEdge = (p2->xPos + p2_hWidth);

					p2->pushBox.l_Edge = p2->xPos - 2;
					p2->pushBox.r_Edge = p2->xPos + 2;
				}
			}

		}
	}

}

void match_Combat_State(playerData *p1, playerData *p2,
		uint8_t p1_Strike_Frame_Active, uint8_t p2_Strike_Frame_Active) {

	if (p1->actionState == ACT_ATTACKING && p1_Strike_Frame_Active) {
		if (p1->playerDir == 'r') {
			if ((p1->hitBox.r_Edge >= p2->leftEdge - 1)
					&& (p1->hitBox.b_Edge >= p2->topEdge)
					&& (p1->hitBox.t_Edge <= p2->bottomEdge)) {
				p2->damage = 10;
			}

		} else {
			if ((p1->hitBox.l_Edge <= p2->rightEdge + 1)
					&& (p1->hitBox.b_Edge >= p2->topEdge)
					&& (p1->hitBox.t_Edge <= p2->bottomEdge)) {
				p2->damage = 10;
			}
		}

	}

	if (p2->actionState == ACT_ATTACKING && p2_Strike_Frame_Active) {

		if (p2->playerDir == 'l') {

			if ((p2->hitBox.l_Edge <= p1->rightEdge + 1)
					&& (p2->hitBox.b_Edge >= p1->topEdge)
					&& (p2->hitBox.t_Edge <= p1->bottomEdge)) {
				p1->damage;
			}

		} else {
			if ((p2->hitBox.l_Edge <= p1->rightEdge + 1)
					&& (p2->hitBox.b_Edge >= p1->topEdge)
					&& (p2->hitBox.t_Edge <= p1->bottomEdge)) {
				p1->damage = 10;
			}
		}
	}
}
// Actively sets both players direction facing eachother
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

// Sets player sprite depending on state and draws player sprite and boxes

/*
 *  [7/16/2026] Optimize this code later so that the pipeline does not evaluate at each and every state/stage whether the player is facing a certain
 * direction or is in a certain action sequence. For example, evaluate once if the player is idle -> if that is false, *then* evaluate the specific action, maybe with the
 * player direction being the first abstraction and evaluation so it doesn't check for every sequence.
 */
void draw_Player(playerData *player) {

	if (player->actionState == ACT_ATTACKING) {
		if (player->state == CROUCHING) {
			if (player->playerDir == 'r') {
				player->character.currentSprite = CROUCH_JAB_R;
			} else {
				player->character.currentSprite = CROUCH_JAB_L;
			}
		} else {
			if (player->playerDir == 'r') {
				player->character.currentSprite = ATTACKING_R;
			} else {
				player->character.currentSprite = ATTACKING_L;
			}
		}

	} else {

		switch (player->state) {

		case IDLE:
			if (player->playerDir == 'r') {
				player->character.currentSprite = IDLE_TEST_R;
			} else {
				player->character.currentSprite = IDLE_TEST_L;
			}
			break;

		case JUMPING:
			if (player->playerDir == 'r') {
				player->character.currentSprite = JUMPING_R;
			} else {
				player->character.currentSprite = JUMPING_R;
			}
			break;

		case CROUCHING:
			if (player->playerDir == 'r') {
				player->character.currentSprite = IDLE_CROUCHING_R;
			} else {
				player->character.currentSprite = IDLE_CROUCHING_L;
			}
			break;
		}
	}

	player->character.currentSprite.offsetX = -10;
	player->character.currentSprite.offsetY = -13;

	// Sprite view
	ssd1306_DrawBitmap(player->xPos + player->character.currentSprite.offsetX,
			player->yPos + player->character.currentSprite.offsetY,
			player->character.currentSprite.data,
			player->character.currentSprite.width,
			player->character.currentSprite.height, White);

	// Hurtbox view
	ssd1306_DrawRectangle(player->leftEdge, player->topEdge, player->rightEdge,
			player->bottomEdge, White);

	// Hitbox view
	if (player->actionState == ACT_ATTACKING) {
		ssd1306_DrawRectangle(player->hitBox.l_Edge, player->hitBox.t_Edge,
				player->hitBox.r_Edge, player->hitBox.b_Edge, White);
	}

	// Pushbox view
	ssd1306_DrawRectangle(player->pushBox.l_Edge, player->pushBox.t_Edge,
			player->pushBox.r_Edge, player->pushBox.b_Edge, White);

}

