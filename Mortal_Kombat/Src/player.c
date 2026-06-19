#include "stm32l476xx.h"
#include "player.h"

// clears player data struct stack to 0
void playerData_Init(playerData *player) {

	// Clears entire player data struct to 0
	memset(player, 0, sizeof(*player));

}

extern const characterProfile SCORPION;
extern const characterProfile SUBZERO;

