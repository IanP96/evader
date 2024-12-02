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
Colour bulletColour = {255, 0, 0};
Colour playerColour = {0, 0, 255};
Colour bgColour = {10, 10, 10};
