A simple game made using C, SDL2 and an input-update-render code design.

# Setup

You need to install **SDL2** first. Depending on the OS, you may need to update the include/library directories in the `Makefile`. Then run
```
make run
```
to play the game (this will create and run the `main` executable).

# How to play

You are the blue square. You must jump between the green platforms to pick up all the coins (yellow squares). The bullets (red squares) will be continuously fired at you, which you must avoid. Falling off all the platforms will also send you to the red "lava" which will end the game.

## Controls

- Arrow keys to move
- <kbd>R</kbd> to restart
- <kbd>Q</kbd> to quit
- <kbd>B</kbd> for bullet hell - drastically increase the rate at which bullets spawn, just for fun.

# References

`audio.c` and `audio.h` were sourced from <a href="https://github.com/jakebesworth/Simple-SDL2-Audio">GitHub</a>, courtesy of Jake Besworth, Lorenzo Mancini, Ted, Eric Boez and Ivan Karlović.