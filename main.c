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
Time bulletFreq;

MovingRect player;
// Which keys are currently pressed
struct Keys {
    bool l, r, u, d;
} keys = {false, false, false, false};

Bullet bullets[maxBullets];
unsigned int numBulletsSpawned;
int nextBulletNum;

MovingRect platforms[numPlatforms];

MovingRect coins[numCoins];
int numCoinsLeft = numCoins;
bool coinsCollected[numCoins];

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

// Destroy SDL renderer and window
void destroy_window(void) {
    printf("Destroying window\n");
    // destroying in reverse order of creation
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    exit(EXIT_SUCCESS);
}

// Game over, destroy window
void game_over(void) {
    printf("Game over\n");
    destroy_window();
}

/**
 * @brief Set initial state before looping
 * 
 */
void setup(void) {

    lastFrameTime = SDL_GetTicks64();

    lastBulletSpawnTime = SDL_GetTicks64();
    nextBulletNum = 0;
    numBulletsSpawned = 0;

    player.pos.x = 0;
    player.pos.y = 0;
    player.w = PLAYER_SIZE;
    player.h = PLAYER_SIZE;
    player.dir.x = 0;
    player.dir.y = 0;

    bulletFreq = maxBulletFreq;

    // Choose locations for coins
    // Which platforms have coins above them
    bool hasCoin[numPlatforms];
    memset(hasCoin, false, sizeof(hasCoin));
    for (int i = 0; i < numCoins; i++)
    {
        int spotsRemaining = numPlatforms - i;
        int nextCoin = arc4random_uniform(spotsRemaining);
        int j = -1, count = -1;
        while (count != nextCoin)
        {
            j++;
            if (!hasCoin[j])
                count++;
        }
        hasCoin[j] = true;
    }

    memset(coinsCollected, false, sizeof(coinsCollected));
    int coinCount = 0;

    // Create platforms, spread in grid with some random variation
    for (size_t i = 0; i < numPlatforms; i++)
    {
        // No movement for platforms
        platforms[i].dir.x = 0;
        platforms[i].dir.y = 0;

        // Positioning
        int col = i % platformGridSize;
        int row = i / platformGridSize;
        platforms[i].pos.x = col * (platformSeparation + platformWidth) + arc4random_uniform(platformSeparation);
        platforms[i].pos.y = row * (platformSeparation + platformHeight) + arc4random_uniform(platformSeparation);

        // Size
        platforms[i].w = platformWidth;
        platforms[i].h = platformHeight;

        // Spawn player above starting platform
        if (row == 0 && col == platformGridSize / 2)
        {
            player.pos = platforms[i].pos;
            player.pos.y -= platformSeparation;
        }

        // Spawn coin if needed
        if (hasCoin[i])
        {
            coins[coinCount].dir.x = 0;
            coins[coinCount].dir.y = 0;
            coins[coinCount].pos = platforms[i].pos;
            coins[coinCount].pos.y -= platformHeight + 1;
            coins[coinCount].pos.x += (platformWidth - coinSize) / 2;
            coins[coinCount].w = coinSize;
            coins[coinCount].h = coinSize;
            coinCount++;
        }
    }
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
        switch (event.key.keysym.sym) {
        case SDLK_ESCAPE:
            printf("Esc pressed\n");
            gameRunning = false;
            break;
        case SDLK_LEFT:
            if (!keys.r) keys.l = true;
            break;
        case SDLK_RIGHT:
            if (!keys.l) keys.r = true;
            break;
        case SDLK_UP:
            if (!keys.d) keys.u = true;
            break;
        case SDLK_DOWN:
            if (!keys.u) keys.d = true;
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

// Spawn a new bullet, crash if no more space for a bullet
void spawn_bullet(void)
{
    size_t i = nextBulletNum;
    nextBulletNum++;
    if (nextBulletNum == maxBullets)
        nextBulletNum = 0;
    if (numBulletsSpawned < maxBullets)
        numBulletsSpawned++;
    Bullet* bullet = bullets + i;

    bullet->movingRect.w = BULLET_SIZE;
    bullet->movingRect.h = BULLET_SIZE;

    // Random bullet spawn location
    OrderedPair pos;
    if (arc4random_uniform(2) == 0)
    {
        // spawn on top/bottom edge
        pos.y = player.pos.y + WINDOW_HEIGHT / 2 * pow(-1, arc4random_uniform(2));
        pos.x = player.pos.x - WINDOW_WIDTH / 2 + arc4random_uniform(WINDOW_WIDTH);
    } else {
        // spawn on left-right edge
        pos.x = player.pos.x + WINDOW_WIDTH / 2 * pow(-1, arc4random_uniform(2));
        pos.y = player.pos.y - WINDOW_HEIGHT / 2 + arc4random_uniform(WINDOW_HEIGHT);
    }
    bullet->movingRect.pos = pos;

    // Bullet goes toward player
    bullet->movingRect.dir = scaled_vector(relative_pos(player.pos, bullet->movingRect.pos), playerHorizontalSpeed);
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
    if (currentTime > lastBulletSpawnTime + bulletFreq)
    {
        lastBulletSpawnTime = currentTime;
        spawnBullet = true;
    }

    if (spawnBullet)
    {
        spawn_bullet();
    }

    // Set player velocity
    player.dir.x = 0;
    if (keys.l) player.dir.x = -playerHorizontalSpeed;
    else if (keys.r) player.dir.x = playerHorizontalSpeed;
    player.dir.y += gravity;
    // Don't exceed terminal velocity
    if (player.dir.y > terminalVelocity)
        player.dir.y = terminalVelocity;
    bool onPlatform = false;
    for (size_t i = 0; i < numPlatforms; i++)
    {
        if (would_collide(player, platforms[i], delta) == EDGE_BOTTOM)
        {
            onPlatform = true;
            player.pos.y = platforms[i].pos.y - PLAYER_SIZE - 0.0001;
            player.dir.y = 0;
            break;
        }
    }
    if (onPlatform && keys.u)
        player.dir.y = -playerJumpSpeed;
    
    // Player collecting coins
    for (size_t i = 0; i < numCoins; i++)
    {
        if (!coinsCollected[i] && would_collide(player, coins[i], delta) != EDGE_NONE)
        {
            // Coin collected
            numCoinsLeft--;
            coinsCollected[i] = true;
            bulletFreq -= (maxBulletFreq - minBulletFreq) / (numCoins - 1);
        }
    }
    
    // Move bullets
    for (size_t i = 0; i < numBulletsSpawned; i++) {
        if (would_collide(player, bullets[i].movingRect, delta) != -1)
        {
            // Player collided with bullet
            game_over();
        }
        move_rect(&(bullets[i].movingRect), delta);
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
    for (size_t i = 0; i < numBulletsSpawned; i++) {
        fill_rect_relative(renderer, bullets[i].movingRect, player.pos);
    }

    // Draw platforms
    set_render_colour(renderer, platformColour);
    for (size_t i = 0; i < numPlatforms; i++)
    {
        fill_rect_relative(renderer, platforms[i], player.pos);
    }

    // Draw coins
    set_render_colour(renderer, coinColour);
    for (int i = 0; i < numCoins; i++)
    {
        if (!coinsCollected[i])
        {
            fill_rect_relative(renderer, coins[i], player.pos);
        }
    }

    // Draw coin display
    for (int i = 0; i < numCoins; i++)
    {
        Colour colour = (i < numCoinsLeft) ? coinColour : greyCoinColour;
        int row = i / coinDisplayGridSize;
        int col = i % coinDisplayGridSize;
        set_render_colour(renderer, colour);
        MovingRect coin;
        coin.pos.x = coinDisplayWidth * (1 + col * 2);
        coin.pos.y = coinDisplayWidth * (1 + row * 2);
        coin.w = coinDisplayWidth;
        coin.h = coinDisplayWidth;
        fill_rect_standard(renderer, coin);
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
