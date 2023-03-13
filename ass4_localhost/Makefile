CC = gcc
CFLAGS= -Wall -pedantic -std=gnu99 -g
.PHONY: all clean
.DEFAULT_GOAL: all
TARGETS = roc2310 control2310 mapper2310
LIBS += -lm -lpthread

all: $(TARGETS)

mapper2310: mapper.o utilities.o output
	$(CC) $(CFLAGS) output/mapper.o output/utilities.o -o mapper2310 $(LIBS)

control2310: airport.o utilities.o output
	$(CC) $(CFLAGS) output/airport.o output/utilities.o -o control2310 $(LIBS)

roc2310: aircraft.o utilities.o output
	$(CC) $(CFLAGS) output/aircraft.o output/utilities.o -o roc2310

mapper.o: mapper.c mapper.h output
	$(CC) $(CFLAGS) mapper.c -c -o output/mapper.o

airport.o: airport.c airport.h output
	$(CC) $(CFLAGS) airport.c -c -o output/airport.o

aircraft.o: aircraft.c aircraft.h output
	$(CC) $(CFLAGS) aircraft.c -c -o output/aircraft.o

utilities.o: utilities.c utilities.h output
	$(CC) $(CFLAGS) utilities.c -c -o output/utilities.o

output:
	mkdir output

clean:
	rm -f $(TARGETS) output/*.o
