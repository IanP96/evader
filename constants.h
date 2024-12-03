#include <stdint.h>

#include "rect.h"
#include "mysdl.h"

typedef struct Bullet {
    MovingRect movingRect;
    // When was bullet spawned
    Time birth;
} Bullet;

int const PLAYER_SIZE = 10;
// player movement in pixels per ms
float const PLAYER_SPEED = 0.4;
// Max num bullets possible to exist at once
int const maxBullets = 1000;
int const BULLET_SIZE = 20;

Colour bulletColour = {200, 0, 0};
Colour playerColour = {0, 0, 255};
// Sky colour
Colour bgColour = {175, 246, 255};
Colour platformColour = {0, 200, 0};

int const platformGridSize = 10;
int const numPlatforms = platformGridSize * platformGridSize;
int const platformHeight = 20;
int const platformWidth = 100;
int const platformSeparation = 200;

float const gravity = 0.01;
float const terminalVelocity = 3;
