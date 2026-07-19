Mortal Kombat Themed STM32 Fighting Game Arcade System (Alpha) 

## Hardware
- STM32L476RG Nucleo Board
- 0.96" OLED (SSD1306, I2C driver)
- Two custom joystick controllers on breadboard
- ADC + DMA routing for simultaneous dual-input polling
- Piezobuzzer for theme music

## Features
- Two-player local fighting
- Health bar system with first to two rounds wins
- Basic collision detection with pushbox, hurtbox and hitboxes
- Custom mortal kombat inspired sprites, mapped dynamically to player boxes and rendered to OLED
- Frame data and animation cycling for an authentic fighting game experience

## Current Status
Alpha release, Core mechanics functional. Next steps:
- Design more sprites for animated frame cycling and lively action sequences
- Add combo system and additional moves
- Implement character selection screen

