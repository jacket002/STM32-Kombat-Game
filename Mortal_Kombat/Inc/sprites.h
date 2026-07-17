#ifndef SPRITES_H
#define SPRITES_H

typedef struct {
     const uint8_t *data;
     uint16_t width;
     uint16_t height;
     uint8_t dataSize;

     uint32_t offsetX;
     uint32_t offsetY;

} tSprite;

extern const tSprite IDLE_TEST_R;

extern const tSprite IDLE_TEST_L;

extern const tSprite IDLE_TEST_L;

extern const tSprite JUMPING_R;

extern const tSprite JUMPING_L;

extern const tSprite ATTACKING_R;

extern const tSprite ATTACKING_L;

extern const tSprite IDLE_CROUCHING_R;

extern const tSprite IDLE_CROUCHING_L;

extern const tSprite CROUCH_JAB_R;

extern const tSprite CROUCH_JAB_L;



#endif
