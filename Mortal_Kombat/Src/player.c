#include "stm32l476xx.h"
#include "player.h"

// clears player data struct stack to 0
void playerData_Init(playerData *player) {

	// Clears entire player data struct to 0
	memset(player, 0, sizeof(*player));

}

// Calculates the max size of frame data array in chracterProfile struct
int character_Read_Frame_Data(playerData *player) {

	uint8_t frames_NumOf = sizeof(player->character.frameData)
			/ sizeof(player->character.frameData[0]);

	return frames_NumOf;

}

void inject_HitBox_Data(playerData *player, int c_Frame) {

	uint8_t totalFrames = character_Read_Frame_Data(player);

	if (c_Frame <= totalFrames) {

		uint32_t boxOffsetX = 5;
		uint32_t boxOffsetY = 5;
		uint32_t boxWidth = 10;
		uint32_t boxHeight = 4;

		if (player->playerDir == 'r') {

			player->hitBox.xPos = player->xPos + boxOffsetX;
			player->hitBox.yPos = player->yPos - boxOffsetY;

			player->hitBox.l_Edge = player->hitBox.xPos - boxWidth / 2;
			player->hitBox.r_Edge = player->hitBox.xPos + boxWidth / 2;

			player->hitBox.t_Edge = player->hitBox.yPos - boxHeight / 2;
			player->hitBox.b_Edge = player->hitBox.yPos + boxHeight / 2;
		} else {

			player->hitBox.xPos = player->xPos - boxOffsetX;
			player->hitBox.yPos = player->yPos - boxOffsetY;

			player->hitBox.l_Edge = player->hitBox.xPos - boxWidth / 2;
			player->hitBox.r_Edge = player->hitBox.xPos + boxWidth / 2;

			player->hitBox.t_Edge = player->hitBox.yPos - boxHeight / 2;
			player->hitBox.b_Edge = player->hitBox.yPos + boxHeight / 2;

		}

	}
}

void update_PlayerHealth(playerData *player) {

	player->health = player->health - player->damage;

	player->damage = 0;

}

int evaluate_Results(playerData *p1, playerData *p2) {

	uint8_t player1_Wins = 1;
	uint8_t player2_Wins = 2;
	uint8_t draw = 0;

	if (p1->health > p2->health) {
		p1->roundsWon += 1;
		return player1_Wins;
	} else if (p2->health > p1->health) {
		p2->roundsWon += 1;
		return player2_Wins;
	}

	return draw;
}

// Flag to finish the match
int is_Match_Finished(playerData *p1, playerData *p2) {

	uint8_t match_finished_flag = 0;

	if (p1->roundsWon == 2 || p2->roundsWon == 2) {
		match_finished_flag = 1;
	}

	return match_finished_flag;

}

const characterProfile SCORPION;
const characterProfile SUBZERO;

