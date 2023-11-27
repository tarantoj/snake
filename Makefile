CC = clang
CFLAGS = -Wall -O3 -march=native
LIBS = -lncurses

snake: main.c
	$(CC) $(CFLAGS) -o snake main.c $(LIBS)

debug: CFLAGS += -g
debug: snake

clean:
	rm -f snake
