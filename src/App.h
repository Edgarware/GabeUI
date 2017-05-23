#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>

#include "Button.h"
#include "MainButton.h"
#include "MenuButton.h"

enum{
    PARSE_STATE_NULL=0,
    PARSE_STATE_MAIN,
    PARSE_STATE_MENU,
    PARSE_STATE_ITEM
};

enum{
    PARSE_MENUITEM_NULL=0,
    PARSE_MENUITEM_QUIT,
    PARSE_MENUITEM_REBOOT,
    PARSE_MENUITEM_SHUTDOWN,
    PARSE_MENUITEM_APPLAUNCH
};

struct ButtonElement{
	char type;
    std::string image;
    std::string app;
    std::string params;
    std::string name;
    char itemtype;
	std::vector<ButtonElement> items;
};

class App{

private:
	SDL_Window *window;
	SDL_Renderer *renderer;
	TTF_Font *font;
	SDL_GameController *controller;
	std::vector<MainButton*> MainButtonList;
	std::vector<MenuButton*> BottomButtonList;
	SDL_Event e;
	bool focus;
	bool quit;
	SDL_Color textColor;
	int selected;
public:
	App();
	int Main(int argc, char** argv);
	bool configParse(std::ifstream *config);
	void goLeft();
	void goRight();
	void goUp();
	void goDown();
	void goSelect();
	void goDeselect();
	void scanForController();
};
