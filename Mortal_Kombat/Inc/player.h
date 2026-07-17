#ifndef PLAYER_H
#define PLAYER_H

#include "sprites.h"

typedef struct {

	// frames array
    uint32_t frameData[10];
    // current character sprite
    tSprite currentSprite;
    // Hurtbox dimensions
    uint32_t boxWidth;
    uint32_t boxHeight;

} characterProfile;

typedef struct {

	int32_t xPos;
	int32_t yPos;

	int32_t l_Edge;
	int32_t r_Edge;

	int32_t t_Edge;
	int32_t b_Edge;

	int32_t amount;

} playerHitBox;

typedef struct {

	int32_t xPos;
	int32_t yPos;

	int32_t l_Edge;
	int32_t r_Edge;

	int32_t t_Edge;
	int32_t b_Edge;

} playerPushBox;

typedef enum {

	ACT_NONE,
	ACT_ATTACKING

} actionState;


// Player data struct
typedef struct {

	int32_t xPos;
	int32_t yPos;
	int32_t yVel;
	int32_t xVel;

	int32_t leftEdge;
	int32_t rightEdge;
	int32_t topEdge;
	int32_t bottomEdge;

	uint8_t roundsWon;
	uint8_t health;
	uint32_t damage;
	uint8_t state;
	actionState actionState;

	playerHitBox hitBox;
	playerPushBox pushBox;

	uint8_t inputHistory[4];
	uint8_t ctrIndex;
	uint8_t buttonReleased;

	characterProfile character;

	char playerDir;

} playerData;

// enumerator for player states
typedef enum {
	IDLE = 0,
	JUMPING = 1,
	BLOCKING = 3,
	CROUCHING = 4
} state ;

#define PLAYER1_STARTING_X_POS 20
#define PLAYER1_STARTING_Y_POS 55 - 6

#define PLAYER2_STARTING_X_POS 65
#define PLAYER2_STARTING_Y_POS 55 - 6

void playerData_Init(playerData *player);
int character_Parse_Frame_Data(playerData *player);
void inject_HitBox_Data(playerData *player, int c_Frame);
void update_PlayerHealth(playerData *player);
int is_Match_Finished(playerData *p1, playerData *p2);
int evaluate_Results(playerData *p1, playerData *p2);


extern const characterProfile SCORPION;
extern const characterProfile SUBZERO;



#endif
