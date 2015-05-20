#ifndef APP_H_
#define APP_H_

#include <string>
#include <vector>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "Button.h"
#include "MainButton.h"
#include "MenuButton.h"

class App{

public:
	SDL_Window *window;
	SDL_Renderer *renderer;

	TTF_Font *font;
	SDL_GameController *controller;
	std::vector<Button*> ButtonList;
	MainButton *Steam;
	MainButton *Plex;
	MenuButton *Options;
	MenuButton *Exit;
	SDL_Event e;
	bool wifiOn;
	bool quit;
	bool focus;
public:
	App();
	int Main(int argc, char** argv);
	void goLeft();
	void goRight();
	void goUp();
	void goDown();
	void goSelect();
	void goDeselect();
	void scanForController();
};

#endif
