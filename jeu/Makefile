CC=gcc
CFLAGS=-Wall 
LDFLAGS=`sdl2-config --cflags --libs` -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

all: Bomberman2.x

%.x: *.o
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS) 

*.o : main.c
	$(CC) $^ -c $(CFLAGS) $(LDFLAGS)  

clean:
	rm -f *~ *.o

realclean: clean
	rm -f *.x