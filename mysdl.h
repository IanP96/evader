#ifndef MYSDL_H
#define MYSDL_H

#include <stdint.h>
#include <SDL2/SDL.h>

#include "rect.h"

typedef struct Colour {
    uint8_t r, g, b;
} Colour;

typedef uint64_t Time;

extern int const WINDOW_WIDTH;
extern int const WINDOW_HEIGHT;

// How long to delay before updating in ms
extern uint32_t const DELAY;

// Set render colour for next draw operation
void set_render_colour(SDL_Renderer* renderer, Colour colour);

// Draw rectangle. (0, 0) will be the centre of the screen.
void fill_rect(SDL_Renderer* renderer, MovingRect rect);

// Draw rectangle with position relative to given pos.
// The given pos will be the considered the centre of the screen
void fill_rect_relative(SDL_Renderer* renderer, MovingRect rect, OrderedPair relativeTo);

#endif // !MYSDL_H
