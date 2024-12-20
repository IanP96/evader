#ifndef GAMEOVER_H
#define GAMEOVER_H

#include <stdbool.h>
#include <SDL2/SDL.h>

/**
 * @brief End game (won or lost).
 * 
 * @param won true if won, false if lost
 */
void game_over_loop(bool won);

#endif
