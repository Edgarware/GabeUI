CC=g++

SDL_CFLAGS := $(shell pkg-config --cflags sdl2 SDL2_image SDL2_ttf)
SDL_LIBS := $(shell pkg-config --libs sdl2 SDL2_image SDL2_ttf)

override CFLAGS += -c -Wall -Wextra $(SDL_CFLAGS)
LDFLAGS = 

all : gabeui

gabeui: App.o MenuItem.o MenuButton.o MainButton.o Button.o Menu.o main.o
	$(CC) $(LDFLAGS) main.o MenuItem.o MenuButton.o MainButton.o Menu.o Button.o App.o -o gabeui $(SDL_LIBS)

main.o: src/main.cpp
	$(CC) $(CFLAGS) src/main.cpp

App.o: src/App.cpp
	$(CC) $(CFLAGS) src/App.cpp

MenuItem.o: src/MenuItem.cpp
	$(CC) $(CFLAGS) src/MenuItem.cpp

MenuButton.o: src/MenuButton.cpp
	$(CC) $(CFLAGS) src/MenuButton.cpp

Menu.o: src/Menu.cpp
	$(CC) $(CFLAGS) src/Menu.cpp

MainButton.o: src/MainButton.cpp
	$(CC) $(CFLAGS) src/MainButton.cpp

Button.o: src/Button.cpp
	$(CC) $(CFLAGS) src/Button.cpp

clean:
	rm -f *o gabeui