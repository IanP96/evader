/**
 * @brief Evader game
 * 
 * Arrow keys to move
 * Q to quit
 * R to restart
 */

// SDL2 wiki: wiki.libsdl.org

#include <stdlib.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

#include "constants.h"
#include "rect.h"
#include "mysdl.h"
#include "gameover.h"
#include "audio.h"

#if ENABLE_LOG
#include <stdio.h>
#endif

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

GameState nextState;

// Time of last frame update in ms
Time lastFrameTime;
// Time of last bullet spawn in ms
Time lastBulletSpawnTime;
// Time to wait before spawning next bullet
Time bulletDelay;

MovingRect player;
// Which keys are currently pressed
struct Keys {
    bool l, r, u, d;
} keys;

Bullet bullets[MAX_BULLETS];
unsigned int numBulletsSpawned;
int nextBulletNum;

MovingRect platforms[NUM_PLATFORMS];
// Big wall of red below all the platforms, touch it and you die
MovingRect lava;

MovingRect coins[NUM_COINS];
int numCoinsLeft;
bool coinsCollected[NUM_COINS];

void log_msg(char* msg) {
    #if ENABLE_LOG
    printf("%s", msg);
    #endif
}

void log_err(char* errMsg) {
    #if ENABLE_LOG
    fprintf(stderr, "%s", errMsg);
    #endif
}

// Initialises SDL, window, renderer. Returns true on success, false on failure
bool init_sdl(void) {

    // Timer not used yet, just in case I want to use it later
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER)) {
        // init failed
        log_err("Error initialising sdl\n");
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
        log_err("Error creating window\n");
        return false;
    }
    renderer = SDL_CreateRenderer(
        window,
        -1, // driver code, use default
        0 // flags
    );
    if (!renderer)
    {
        log_err("Error creating renderer\n");
        return false;
    }
    
    return true;
}

// Destroy SDL renderer and window, end music, exit
void exit_game(void) {
    log_msg("Destroying window\n");
    // destroying in reverse order of creation
    // endAudio();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    exit(EXIT_SUCCESS);
}

// Game over, won or lost
void game_over(bool won) {
    log_msg("Game over\n");
    nextState = won ? STATE_GAME_OVER_WON : STATE_GAME_OVER_LOST;
}

/**
 * @brief Set initial state before looping
 * 
 */
void setup(void) {

    initAudio();
    playMusic("assets/sound/bgm.wav", soundVolume);

    Time startTime = SDL_GetTicks64();
    lastFrameTime = startTime;
    nextState = STATE_CONTINUE;

    lastBulletSpawnTime = startTime;
    nextBulletNum = 0;
    numBulletsSpawned = 0;

    memset(&keys, false, sizeof(keys));

    player.pos.x = 0;
    player.pos.y = 0;
    player.w = PLAYER_SIZE;
    player.h = PLAYER_SIZE;
    player.dir.x = 0;
    player.dir.y = 0;

    bulletDelay = maxBulletDelay;

    // Choose locations for coins
    // Which platforms have coins above them
    bool hasCoin[NUM_PLATFORMS];
    memset(hasCoin, false, sizeof(hasCoin));
    for (int i = 0; i < NUM_COINS; i++)
    {
        int spotsRemaining = NUM_PLATFORMS - i;
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
    numCoinsLeft = NUM_COINS;

    // Create platforms, spread in grid with some random variation
    for (size_t i = 0; i < NUM_PLATFORMS; i++)
    {
        // No movement for platforms
        platforms[i].dir.x = 0;
        platforms[i].dir.y = 0;

        // Positioning
        int col = i % PLATFORM_GRID_SIZE;
        int row = i / PLATFORM_GRID_SIZE;
        platforms[i].pos.x = col * (platformSeparation + platformWidth) + arc4random_uniform(platformSeparation);
        platforms[i].pos.y = row * (platformSeparation + platformHeight) + arc4random_uniform(platformSeparation);

        // Size
        platforms[i].w = platformWidth;
        platforms[i].h = platformHeight;

        // Spawn player above starting platform
        if (row == 0 && col == PLATFORM_GRID_SIZE / 2)
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

    // Create lava
    lava.pos.y = PLATFORM_GRID_SIZE * (platformSeparation + platformHeight) + WINDOW_HEIGHT; // add
    // window height to ensure it is out of view
    int platformAreaWidth = PLATFORM_GRID_SIZE * (platformSeparation + platformWidth);
    lava.pos.x = -platformAreaWidth;
    lava.w = platformAreaWidth * 3;
    lava.h = lava.w;
    lava.dir.x = 0;
    lava.dir.y = 0;
}

void process_input(void) {
    SDL_Event event;
    SDL_PollEvent(&event);
    
    switch (event.type) {
    case SDL_QUIT: // click x button on window
        log_msg("Quit event detected\n");
        nextState = STATE_EXIT;
        break;
    case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
        case SDLK_q:
            log_msg("Q pressed\n");
            nextState = STATE_EXIT;
            break;
        case SDLK_r:
            nextState = STATE_MAIN;
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
    if (nextBulletNum == MAX_BULLETS)
        nextBulletNum = 0;
    if (numBulletsSpawned < MAX_BULLETS)
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
    if (currentTime > lastBulletSpawnTime + bulletDelay)
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
    for (size_t i = 0; i < NUM_PLATFORMS; i++)
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
    
    // Collecting coins
    for (size_t i = 0; i < NUM_COINS; i++)
    {
        if (!coinsCollected[i] && would_collide(player, coins[i], delta) != EDGE_NONE)
        {
            // Coin collected
            numCoinsLeft--;
            coinsCollected[i] = true;
            if (NUM_COINS != 1)
            {
                // Avoid division by 0
                bulletDelay -= (maxBulletDelay - minBulletDelay) / (NUM_COINS - 1);
            }
            if (numCoinsLeft == 0)
                game_over(true);
            else
                playSound("assets/sound/coin.wav", soundVolume);
        }
    }
    
    // Move bullets
    for (size_t i = 0; i < numBulletsSpawned; i++) {
        if (
            would_collide(player, bullets[i].movingRect, delta) != EDGE_NONE
            && nextState == STATE_CONTINUE
        )
        {
            // Player collided with bullet
            game_over(false);
        }
        move_rect(&(bullets[i].movingRect), delta);
    }

    // Collision with lava
    if (would_collide(player, lava, delta) != EDGE_NONE)
    {
        game_over(false);
    }
    
    // Move player
    move_rect(&player, delta);
}

void draw_smile(void) {
    MovingRect smile[5];
    // Left eye
    smile[0].pos.x = -2;
    smile[0].pos.y = -2;
    smile[0].w = 1;
    smile[0].h = 1;
    // Right eye
    smile[1].pos.x = 1;
    smile[1].pos.y = -2;
    smile[1].w = 1;
    smile[1].h = 1;
    // Left side of mouth
    smile[2].pos.x = -2;
    smile[2].pos.y = 0;
    smile[2].w = 1;
    smile[2].h = 2;
    // Right side of mouth
    smile[3].pos.x = 1;
    smile[3].pos.y = 0;
    smile[3].w = 1;
    smile[3].h = 2;
    // Bottom of mouth
    smile[4].pos.x = -1;
    smile[4].pos.y = 1;
    smile[4].w = 2;
    smile[4].h = 1;
    // Draw
    set_render_colour(renderer, faceColour);
    for (int i = 0; i < 5; i++)
    {
        smile[i].dir.x *= facePixelSize;
        smile[i].dir.y *= facePixelSize;
        smile[i].pos.x *= facePixelSize;
        smile[i].pos.y *= facePixelSize;
        smile[i].w *= facePixelSize;
        smile[i].h *= facePixelSize;
        fill_rect(renderer, smile[i]);
    }
}

void render(SDL_Renderer* renderer) {
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
    for (size_t i = 0; i < NUM_PLATFORMS; i++)
    {
        fill_rect_relative(renderer, platforms[i], player.pos);
    }

    // Draw lava
    set_render_colour(renderer, bulletColour);
    fill_rect_relative(renderer, lava, player.pos);

    // Draw coins
    set_render_colour(renderer, coinColour);
    for (int i = 0; i < NUM_COINS; i++)
    {
        if (!coinsCollected[i])
        {
            fill_rect_relative(renderer, coins[i], player.pos);
        }
    }

    // Draw coin display
    for (int i = 0; i < NUM_COINS; i++)
    {
        Colour colour = (i < numCoinsLeft) ? coinColour : greyCoinColour;
        int row = i / COIN_DISPLAY_GRID_SIZE;
        int col = i % COIN_DISPLAY_GRID_SIZE;
        set_render_colour(renderer, colour);
        MovingRect coin;
        coin.pos.x = coinDisplayWidth * (1 + col * 2);
        coin.pos.y = coinDisplayWidth * (1 + row * 2);
        coin.w = coinDisplayWidth;
        coin.h = coinDisplayWidth;
        fill_rect_standard(renderer, coin);
    }

    if (nextState == STATE_GAME_OVER_WON)
    {
        draw_smile();
    }

    SDL_RenderPresent(renderer); // buffer swap
}

void main_loop(SDL_Renderer* renderer) {
    setup();
    while (nextState == STATE_CONTINUE)
    {
        SDL_Delay(DELAY); // delay to avoid high cpu consumption
        process_input();
        update();
        render(renderer);
    }
    endAudio();
}

int main(void) {
    #if !ENABLE_LOG
    int devnull = open("/dev/null", O_WRONLY);
    dup2(devnull, 1);
    dup2(devnull, 2);
    close(devnull);
    #endif
    log_msg("Game running\n");
    if (!init_sdl()) {
        exit(EXIT_FAILURE);
    }
    nextState = STATE_MAIN;

    while (1)
    {
        switch (nextState)
        {
        case STATE_EXIT:
            exit_game();
            return 0;
        case STATE_MAIN:
            main_loop(renderer);
            break;
        case STATE_GAME_OVER_WON:
            game_over_loop(true);
            break;
        case STATE_GAME_OVER_LOST:
            game_over_loop(false);
            break;
        default:
            break;
        }
    }
}
