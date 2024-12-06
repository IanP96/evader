#include <stdint.h>

#include <SDL2/SDL.h>

#include "rect.h"
#include "mysdl.h"
#include "constants.h"

unsigned int const soundVolume = SDL_MIX_MAXVOLUME * 2 / 3;

int const PLAYER_SIZE = 10;
// player horizontal movement in pixels per ms
float const playerHorizontalSpeed = 0.4;
// Vertical player speed after jump
float const playerJumpSpeed = 0.6;
int const BULLET_SIZE = 20;
int const coinSize = 20;

Colour bulletColour = {200, 0, 0};
Colour playerColour = {0, 0, 255};
// Sky colour
Colour bgColour = {205, 246, 255};
Colour platformColour = {0, 200, 0};
Colour coinColour = {230, 230, 0};
Colour greyCoinColour = {20, 20, 20};

int const platformHeight = 20;
int const platformWidth = 100;
int const platformSeparation = 200;

int const coinDisplayWidth = 10;

float const gravity = 0.01;
float const terminalVelocity = 3;

// Min time to wait before spawning bullet in ms
int const minBulletDelay = 500;
// Max time to wait before spawning bullet in ms
int const maxBulletDelay = 5000;
