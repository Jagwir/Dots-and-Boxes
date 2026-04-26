CC = gcc
CFLAGS = -Wall -pthread

all: game

game: main.o game.o board.o bot.o network.o
	$(CC) $(CFLAGS) main.o game.o board.o bot.o network.o -o game

main.o: main.c game.h network.h
	$(CC) $(CFLAGS) -c main.c

game.o: game.c game.h board.h bot.h network.h
	$(CC) $(CFLAGS) -c game.c

board.o: board.c board.h
	$(CC) $(CFLAGS) -c board.c

bot.o: bot.c bot.h board.h
	$(CC) $(CFLAGS) -c bot.c

network.o: network.c network.h
	$(CC) $(CFLAGS) -c network.c

clean:
	rm -f *.o game
