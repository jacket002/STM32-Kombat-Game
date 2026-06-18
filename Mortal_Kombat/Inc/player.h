#ifndef PLAYER_H
#define PLAYER_H

typedef struct {

	uint32_t xPos;
	uint32_t yPos;
	uint32_t yVel;

	uint8_t wins;
	uint8_t health;
	uint32_t damage;
	uint8_t state;

	uint8_t inputHistory[4];

} playerData;

typedef enum {
	IDLE = 0,
	JUMPING = 1,
	ATTACKING = 2,
	BLOCKING = 3,
	CROUCHING = 4
} state ;



void playerData_Init(playerData *player);

#endif
