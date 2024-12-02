// wiki: wiki.libsdl.org

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <signal.h>

#include "constants.h"
#include "rect.h"
#include "mysdl.h"

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
bool gameRunning = false;
// Time of last frame update in ms
Time lastFrameTime;
// Time of last bullet spawn in ms
Time lastBulletSpawnTime;
// Time to wait before spawning next bullet
Time bulletSpawnDelay;

MovingRect player;
// Which keys are currently pressed
struct Keys {
    bool l, r, u, d;
} keys = {false, false, false, false};
Bullet bullets[maxBullets];
bool bulletsUsed[maxBullets];

// returns 1 on success, 0 on failure
bool init_window(void) {

    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        // init failed
        fprintf(stderr, "Error initialising sdl\n");
        return 0;
    }

    window = SDL_CreateWindow(
        "Evader", // title
        SDL_WINDOWPOS_CENTERED, // centre of screen x
        SDL_WINDOWPOS_CENTERED, // centre y
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0 //SDL_WINDOW_BORDERLESS // flags
    );
    if (!window)
    {
        fprintf(stderr, "Error creating window\n");
        return 0;
    }
    renderer = SDL_CreateRenderer(
        window,
        -1, // driver code, use default
        0 // flags
    );
    if (!renderer)
    {
        fprintf(stderr, "Error creating renderer\n");
        return 0;
    }
    
    return 1;
}

void process_input(void) {
    SDL_Event event;
    SDL_PollEvent(&event);
    
    switch (event.type) {
    case SDL_QUIT: // click x button on window
        printf("Quit event detected\n");
        gameRunning = false;
        break;
    case SDL_KEYDOWN:
        keys.l = false;
        keys.r = false;
        keys.u = false;
        keys.d = false;
        switch (event.key.keysym.sym) {
        case SDLK_ESCAPE:
            printf("Esc pressed\n");
            gameRunning = false;
            break;
        case SDLK_LEFT:
            keys.l = true;
            break;
        case SDLK_RIGHT:
            keys.r = true;
            break;
        case SDLK_UP:
            keys.u = true;
            break;
        case SDLK_DOWN:
            keys.d = true;
            break;
        default:
            break;
        }
        break;
    case SDL_KEYUP:
        switch (event.key.keysym.sym) {
        case SDLK_LEFT:
            keys.l = false;
            break;
        case SDLK_RIGHT:
            keys.r = false;
            break;
        case SDLK_UP:
            keys.u = false;
            break;
        case SDLK_DOWN:
            keys.d = false;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

// Destroy SDL renderer and window
void destroy_window(void) {
    printf("Destroying window\n");
    // destroying in reverse order of creation
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

/**
 * @brief Set initial state before looping
 * 
 */
void setup(void) {
    lastFrameTime = SDL_GetTicks64();
    lastBulletSpawnTime = SDL_GetTicks64();
    memset(bulletsUsed, false, sizeof(bulletsUsed));
    player.pos.x = 0;
    player.pos.y = 0;
    player.w = PLAYER_SIZE;
    player.h = PLAYER_SIZE;
    player.dir.x = 0;
    player.dir.y = 0;

    bulletSpawnDelay = 1000;
}

// Spawn a new bullet, crash if no more space for a bullet
void spawn_bullet(Time time)
{
    printf("Spawning bullet\n");
    size_t i;
    for (i = 0; i < maxBullets && bulletsUsed[i]; i++);
    if (i == maxBullets)
    {
        fprintf(stderr, "Too many bullets\n");
        destroy_window();
    }
    bulletsUsed[i] = true;
    Bullet* bullet = bullets + i;
    bullet->birth = time;

    bullet->movingRect.w = PLAYER_SIZE;
    bullet->movingRect.h = PLAYER_SIZE;

    // todo more advanced logic for bullet spawn location
    bullet->movingRect.pos.x = player.pos.x + WINDOW_WIDTH * 0.5;
    bullet->movingRect.pos.y = player.pos.y + WINDOW_HEIGHT * 0.5;

    // Bullet goes toward player
    bullet->movingRect.dir = scaled_vector(relative_pos(player.pos, bullet->movingRect.pos), PLAYER_SPEED);
}

/**
 * @brief Update game state after getting input
 * 
 */
void update(void) {

    Time currentTime = SDL_GetTicks64();
    Time delta = currentTime - lastFrameTime;
    lastFrameTime = currentTime;
    // Should I spawn a bullet on this iteration?
    bool spawnBullet = false;
    if (currentTime > lastBulletSpawnTime + bulletSpawnDelay)
    {
        lastBulletSpawnTime = currentTime;
        spawnBullet = true;
    }

    if (spawnBullet)
    {
        spawn_bullet(currentTime);
    }

    // move player
    player.dir.x = 0;
    player.dir.y = 0;
    if (keys.l)
    {
        player.dir.x = -PLAYER_SPEED;
    } else if (keys.r)
    {
        player.dir.x = PLAYER_SPEED;
    } else if (keys.u)
    {
        player.dir.y = -PLAYER_SPEED;
    } else if (keys.d)
    {
        player.dir.y = PLAYER_SPEED;
    }

    // Move bullets
    for (size_t i = 0; i < maxBullets; i++) {
        if (bulletsUsed[i])
        {
            move_rect(&(bullets[i].movingRect), delta);
        }
    }

    // Move player
    move_rect(&player, delta);
}

void render(void) {
    set_render_colour(renderer, bgColour);
    SDL_RenderClear(renderer);

    // Draw player
    set_render_colour(renderer, playerColour);
    fill_rect_relative(renderer, player, player.pos); // always draw player at centre of screen

    // Draw bullets
    set_render_colour(renderer, bulletColour);
    for (size_t i = 0; i < maxBullets; i++) {
        if (bulletsUsed[i])
        {
            fill_rect_relative(renderer, bullets[i].movingRect, player.pos);
        }
    }

    SDL_RenderPresent(renderer); // buffer swap
}

int main(void) {
    printf("Game running\n");
    gameRunning = init_window();

    setup();
    while (gameRunning)
    {
        SDL_Delay(DELAY); // delay to avoid high cpu consumption
        process_input();
        update();
        render();
    }

    destroy_window();
    
    return 0;
}
