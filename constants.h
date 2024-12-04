#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <stdint.h>

#include "rect.h"
#include "mysdl.h"

typedef struct Bullet {
    MovingRect movingRect;
} Bullet;

typedef enum GameState {
    STATE_CONTINUE, STATE_MAIN, STATE_GAME_OVER_WON, STATE_GAME_OVER_LOST, STATE_EXIT
} GameState;

extern unsigned int const soundVolume;

extern int const PLAYER_SIZE;
// player horizontal movement in pixels per ms
extern float const playerHorizontalSpeed;
// Vertical player speed after jump
extern float const playerJumpSpeed;
// Max num bullets possible to exist at once.
// Once this limit is reached, new bullets will replace the oldest bullets
#define MAX_BULLETS 20
extern int const BULLET_SIZE;
extern int const coinSize;

extern Colour bulletColour;
extern Colour playerColour;
// Sky colour
extern Colour bgColour;
extern Colour platformColour;
extern Colour coinColour;
extern Colour greyCoinColour;

// Number of platforms per row/column
#define PLATFORM_GRID_SIZE 10
#define NUM_PLATFORMS (PLATFORM_GRID_SIZE * PLATFORM_GRID_SIZE)
extern int const platformHeight;
extern int const platformWidth;
extern int const platformSeparation;

#define COIN_DISPLAY_GRID_SIZE 5
extern int const coinDisplayWidth;
#define NUM_COINS (COIN_DISPLAY_GRID_SIZE * COIN_DISPLAY_GRID_SIZE)

extern float const gravity;
extern float const terminalVelocity;

// Min time to wait before spawning bullet in ms
extern int const minBulletFreq;
// Max time to wait before spawning bullet in ms
extern int const maxBulletFreq;

#endif
