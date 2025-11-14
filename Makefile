CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -Iinclude `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs` -lm -lSDL2_ttf

SRC = src/main.c src/engine.c src/ecs.c src/weapon.c src/enemy.c
OBJ = $(SRC:.c=.o)

TARGET = game

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(OBJ) $(TARGET)
