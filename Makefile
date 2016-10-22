CC=gcc
CFLAGS=-O3 -march=native
OBJ=proclst.o

all: proclst

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

proclst: proclst.o
	$(CC) -o proclst proclst.o

clean:
	rm proclst proclst.o
