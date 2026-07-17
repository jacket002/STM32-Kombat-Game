#ifndef PHYSICS_H
#define PHYSICS_H

#include "player.h"

void update_PlayerPhysics(playerData *player, uint8_t S1_Active);
void player_Stage(playerData *p1, playerData *p2);
void player_Jump(playerData *player);
int player_Attack(playerData *player);
void draw_Player(playerData *player);
void update_PlayerDirections(playerData *p1, playerData *p2);
void match_Combat_State(playerData *p1, playerData *p2, uint8_t p1_Strike_Frame_Active, uint8_t p2_Strike_Frame_Active);


// Controller 1 GPIO definitions
#define c1_L1_Active !(GPIOA->IDR & GPIO_IDR_ID5)
#define c1_R1_Active !(GPIOA->IDR & GPIO_IDR_ID4)
#define c1_J1_Active !(GPIOA->IDR & GPIO_IDR_ID6)
#define c1_S1_Active !(GPIOA->IDR & GPIO_IDR_ID7)

// Controller 2 GPIO definitions-
#define c2_L1_Active !(GPIOB->IDR & GPIO_IDR_ID5)
#define c2_R1_Active !(GPIOB->IDR & GPIO_IDR_ID4)
#define c2_J1_Active !(GPIOB->IDR & GPIO_IDR_ID6)
#define c2_S1_Active !(GPIOB->IDR & GPIO_IDR_ID7)

#define SCREEN_FLOOR 60
#define SCREEN_CEILING 2
#define SCREEN_CENTER_X 64 // 128div2
#define SCREEN_CENTER_Y 32 // 64div2

#endif
