#OS Specific Defines
#ifeq ($(OS),Windows_NT)
CC=cl.exe
LINK=link.exe
LDFLAGS=/LIBPATH:SDL2\lib\x64 /SUBSYSTEM:CONSOLE /OUT:
CFLAGS=/c /EHsc /ISDL2\include
RM=del
EXEC_NAME=gabeui.exe
SDL_LIBS=SDL2.lib SDL2_ttf.lib SDL2_image.lib SDL2main.lib
#endif
#else
#	OS := $(shell uname)
#	ifeq ($(OS),Linux)
#		CC=g++
#		LINK=g++
#		SDL_CFLAGS := $(shell pkg-config --cflags sdl2 SDL2_image SDL2_ttf)
#		SDL_LIBS := $(shell pkg-config --libs sdl2 SDL2_image SDL2_ttf)
#		override CFLAGS += -c -Wall -Wextra $(SDL_CFLAGS)
#		LDFLAGS = -o 
#	endif
#	ifeq ($(OS),Darwin)
#		$(error OSX not currently supported)
#	endif
#	else
#	endif
#endif

all : $(EXEC_NAME)

$(EXEC_NAME): App.obj MenuItem.obj MenuButton.obj MainButton.obj Button.obj Menu.obj jsmn.obj main.obj
	$(LINK) $(LDFLAGS)$(EXEC_NAME) main.obj MenuItem.obj MenuButton.obj MainButton.obj Menu.obj jsmn.obj Button.obj App.obj $(SDL_LIBS)

main.obj: src/main.cpp
	$(CC) $(CFLAGS) src/main.cpp

App.obj: src/App.cpp
	$(CC) $(CFLAGS) src/App.cpp

MenuItem.obj: src/MenuItem.cpp
	$(CC) $(CFLAGS) src/MenuItem.cpp

MenuButton.obj: src/MenuButton.cpp
	$(CC) $(CFLAGS) src/MenuButton.cpp

Menu.obj: src/Menu.cpp
	$(CC) $(CFLAGS) src/Menu.cpp

MainButton.obj: src/MainButton.cpp
	$(CC) $(CFLAGS) src/MainButton.cpp

Button.obj: src/Button.cpp
	$(CC) $(CFLAGS) src/Button.cpp

jsmn.obj: src/jsmn/jsmn.c
	$(CC) $(CFLAGS) src/jsmn/jsmn.c

clean:
	$(RM) *obj $(EXEC_NAME)