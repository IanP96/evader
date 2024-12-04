#include <stdint.h>

#include "rect.h"
#include "mysdl.h"

typedef struct Bullet {
    MovingRect movingRect;
    // When was bullet spawned
    Time birth;
} Bullet;

int const PLAYER_SIZE = 10;
// player horizontal movement in pixels per ms
float const playerHorizontalSpeed = 0.4;
// Vertical player speed after jump
float const playerJumpSpeed = 0.6;
// Max num bullets possible to exist at once
int const maxBullets = 1000;
int const BULLET_SIZE = 20;
int const coinSize = 20;

Colour bulletColour = {200, 0, 0};
Colour playerColour = {0, 0, 255};
// Sky colour
Colour bgColour = {205, 246, 255};
Colour platformColour = {0, 200, 0};
Colour coinColour = {230, 230, 0};
Colour greyCoinColour = {20, 20, 20};

int const platformGridSize = 10;
int const numPlatforms = platformGridSize * platformGridSize;
int const platformHeight = 20;
int const platformWidth = 100;
int const platformSeparation = 200;

int const coinDisplayGridSize = 5;
int const coinDisplayWidth = 8;
int const numCoins = coinDisplayGridSize * coinDisplayGridSize;

float const gravity = 0.01;
float const terminalVelocity = 3;
