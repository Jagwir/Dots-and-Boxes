CC = gcc
CFLAGS = -Wall

all: game

game: main.o game.o board.o bot.o
	$(CC) $(CFLAGS) main.o game.o board.o bot.o -o game

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

game.o: game.c
	$(CC) $(CFLAGS) -c game.c

board.o: board.c
	$(CC) $(CFLAGS) -c board.c

bot.o: bot.c
	$(CC) $(CFLAGS) -c bot.c

clean:
	rm -f *.o game