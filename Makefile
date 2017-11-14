RM=rm

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	CC=gcc
	SDL_CFLAGS := $(shell pkg-config --cflags sdl2 SDL2_image SDL2_ttf)
	SDL_LIBS := $(shell pkg-config --libs sdl2 SDL2_image SDL2_ttf)
endif
ifeq ($(UNAME_S),Darwin)
	CC=clang
	SDL_CFLAGS := -F/Library/Frameworks -lSDL2 -lSDL2_image -lSDL_ttf
	SDL_LIBS := -F/Library/Frameworks -framework SDL2 -framework SDL2_image -framework SDL2_ttf -framework CoreServices -framework Carbon
endif

override CFLAGS += -DDEBUG -g -O0 -c -Isrc/inc -Isrc -Wall -Wextra $(SDL_CFLAGS)
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