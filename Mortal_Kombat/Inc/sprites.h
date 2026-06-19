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



#endif
