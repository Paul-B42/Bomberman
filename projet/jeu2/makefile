CC = gcc
CFLAGS = -Wall
LDFLAGS = `sdl2-config --cflags --libs` -lSDL2 -lSDL2_image

SRC = main.c
OBJ = $(SRC:.c=.o)

EXEC = Bomberman.x

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(LDFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

mrproper: clean
	rm -f $(EXEC)
