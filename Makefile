CC = gcc
CFLAGS = `pkg-config --cflags gtk+-3.0` -Iinclude
LIBS = `pkg-config --libs gtk+-3.0`
SRC = src/main.c src/gui.c src/emoji.c src/callback.c
OBJ = $(SRC:src/%.c=obj/%.o)
TARGET = bin/emojix

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
