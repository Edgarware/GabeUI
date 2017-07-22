CC=gcc
RM=rm

SDL_CFLAGS := $(shell pkg-config --cflags sdl2 SDL2_image SDL2_ttf)
SDL_LIBS := $(shell pkg-config --libs sdl2 SDL2_image SDL2_ttf)

override CFLAGS += -DDEBUG -g -c -Isrc/inc -Isrc -Wall $(SDL_CFLAGS)
LDFLAGS = 

SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, %.o, $(SRC))

all : gabeui

gabeui: $(OBJ) jsmn.o SDL_FontCache.o
	$(CC) $(LDFLAGS) $(OBJ) jsmn.o SDL_FontCache.o -o gabeui $(SDL_LIBS)

%.o: src/%.c
	$(CC) $(CFLAGS) $< -o $@

jsmn.o: src/jsmn/jsmn.c
	$(CC) $(CFLAGS) src/jsmn/jsmn.c

SDL_FontCache.o: src/fontcache/SDL_FontCache.c
	$(CC) $(CFLAGS) src/fontcache/SDL_FontCache.c

clean:
	$(RM) -f *o gabeui