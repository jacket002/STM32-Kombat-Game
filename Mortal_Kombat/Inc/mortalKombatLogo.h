/* bitmap.h or top of main.c */
#ifndef MORTALKOMBATLOGO_H_
#define MORTALKOMBATLOGO_H_

typedef struct {
     const uint8_t *data;
     uint16_t width;
     uint16_t height;
     uint8_t dataSize;
} tImage;


extern const tImage mortalKombatLogo; // 128 * 64 / 8 = 1024 bytes

#endif
