CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -Iinclude `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs` -lm -lSDL2_ttf

# Tous les .c dans src/ (récursif)
SRC     = $(shell find src -name "*.c")
OBJ     = $(SRC:.c=.o)
DEP     = $(OBJ:.o=.d)

TARGET  = game

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

# Génération des .o avec fichiers de dépendances .d
src/%.o: src/%.c
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -f $(OBJ) $(DEP) $(TARGET)

-include $(DEP)
