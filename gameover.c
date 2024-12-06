#include <stdbool.h>

#include "gameover.h"
#include "constants.h"
#include "audio.h"

GameState nextState;

void game_over_process_input(void) {
    SDL_Event event;
    SDL_PollEvent(&event);
    
    switch (event.type) {
    case SDL_QUIT: // e.g. click x button on window, Ctrl+C
        printf("Quit event detected\n");
        nextState = STATE_EXIT;
        break;
    case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
        case SDLK_q:
            printf("Q pressed\n");
            nextState = STATE_EXIT;
            break;
        case SDLK_r:
            nextState = STATE_MAIN;
            break;
        }
    default:
        break;
    }
}

void game_over_update(void) {}

void game_over_render(void) {}

void game_over_loop(bool won) {

    nextState = STATE_CONTINUE;

    // Play victory/death music
    initAudio();
    char* sound = won ? "assets/sound/win.wav" : "assets/sound/death.wav";
    playSound(sound, soundVolume * 1.1);

    while (nextState == STATE_CONTINUE)
    {
        SDL_Delay(DELAY); // delay to avoid high cpu consumption
        game_over_process_input();
        game_over_update();
        game_over_render();
    }
    endAudio();
}
