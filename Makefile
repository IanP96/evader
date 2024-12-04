CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=gnu99 -I/opt/homebrew/include
LDFLAGS = -L/opt/homebrew/lib -lSDL2 -lm
.PHONY: clean

.DEFAULT_GOAL := main

main: main.o constants.o rect.o mysdl.o gameover.o audio.o
main.o: main.c constants.h rect.h mysdl.h gameover.h audio.h

run:
	make
	./main

clean:
	rm -f main *.o