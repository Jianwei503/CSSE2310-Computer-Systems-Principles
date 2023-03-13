CC=gcc
CFLAGS=-std=gnu99 -Wall -pedantic

all: 2310A 2310B 2310dealer

path.o: path.c path.h
	$(CC) $(CFLAGS) -c path.c -o path.o

deck.o: deck.c deck.h
	$(CC) $(CFLAGS) -c deck.c -o deck.o

player.o: player.c player.h
	$(CC) $(CFLAGS) -c player.c -o player.o

dealer.o: dealer.c dealer.h
	$(CC) $(CFLAGS) -c dealer.c -o dealer.o

gamer.o: gamer.c gamer.h
	$(CC) $(CFLAGS) -c gamer.c -o gamer.o

2310A: gamer.o player.o path.o gamerA.c
	$(CC) $(CFLAGS) gamer.o player.o path.o gamerA.c -o 2310A

2310B: gamer.o player.o path.o gamerB.c
	$(CC) $(CFLAGS) gamer.o player.o path.o gamerB.c -o 2310B

2310dealer: deck.o path.o player.o dealer.o
	$(CC) $(CFLAGS) deck.o path.o player.o dealer.o -o 2310dealer

clean:
	rm -rf path.o deck.o player.o dealer.o gamer.o 2310A 2310B 2310dealer
