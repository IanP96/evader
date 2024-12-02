CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=gnu99 -I/opt/homebrew/include
LDFLAGS = -L/opt/homebrew/lib -lSDL2 -lm
.PHONY: clean

.DEFAULT_GOAL := main

main: main.o rect.o mysdl.o
main.o: main.c constants.h rect.h mysdl.h
rect.o: rect.c rect.h
mysdl.o: mysdl.c mysdl.h

run:
	make
	./main

clean:
	rm -f main *.o