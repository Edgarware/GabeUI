CC=g++
RM=rm

SDL_CFLAGS := $(shell pkg-config --cflags sdl2 SDL2_image SDL2_ttf)
SDL_LIBS := $(shell pkg-config --libs sdl2 SDL2_image SDL2_ttf)

override CFLAGS += -DDEBUG -g -c -Wall $(SDL_CFLAGS)
LDFLAGS = 

SRC = $(wildcard src/*.cpp)
OBJ = $(patsubst src/%.cpp, %.o, $(SRC))

all : gabeui

gabeui: $(OBJ) jsmn.o
	$(CC) $(LDFLAGS) $(OBJ) jsmn.o -o gabeui $(SDL_LIBS)

%.o: src/%.cpp
	$(CC) $(CFLAGS) $< -o $@

jsmn.o: src/jsmn/jsmn.c
	$(CC) $(CFLAGS) src/jsmn/jsmn.c

clean:
	$(RM) -f *o gabeui