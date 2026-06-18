#ifndef PLAYER_H
#define PLAYER_H

// Player data struct
typedef struct {

	int32_t xPos;
	int32_t yPos;
	int32_t yVel;

	int32_t leftEdge;
	int32_t rightEdge;
	int32_t topEdge;
	int32_t bottomEdge;

	uint8_t wins;
	uint8_t health;
	uint32_t damage;
	uint8_t state;

	uint8_t inputHistory[4];

} playerData;

// enumerator for player states
typedef enum {
	IDLE = 0,
	JUMPING = 1,
	ATTACKING = 2,
	BLOCKING = 3,
	CROUCHING = 4
} state ;


#define PLAYER1_STARTING_X_POS 20
#define PLAYER1_STARTING_Y_POS 50

#define PLAYER2_STARTING_X_POS 65
#define PLAYER2_STARTING_Y_POS 50

void playerData_Init(playerData *player);

#endif
