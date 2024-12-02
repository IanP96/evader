#include <SDL2/SDL.h>

#include "mysdl.h"
#include "rect.h"

int const WINDOW_WIDTH = 800;
int const WINDOW_HEIGHT = 600;
uint32_t const DELAY = 10;

void set_render_colour(SDL_Renderer* renderer, Colour colour) {
    SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, 255);
}

void fill_rect(SDL_Renderer* renderer, MovingRect rect) {
    SDL_Rect sdlRect = {
        .x = (int)rect.pos.x, .y = (int)rect.pos.y, 
        .w = (int)rect.w, .h = (int)rect.h
    };
    sdlRect.x += WINDOW_WIDTH / 2;
    sdlRect.y += WINDOW_HEIGHT / 2;
    SDL_RenderFillRect(renderer, &sdlRect);
}

void fill_rect_relative(SDL_Renderer* renderer, MovingRect rect, OrderedPair relativeTo) {
    rect.pos = relative_pos(rect.pos, relativeTo);
    fill_rect(renderer, rect);
}
