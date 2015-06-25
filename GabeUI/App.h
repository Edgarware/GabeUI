#ifndef APP_H_
#define APP_H_

#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

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

struct MainButtonElements{
    std::string image;
    std::string app;
    std::string params;
};

struct MenuItemElements{
    std::string name;
    char type;
    std::string app;
    std::string params;

};

struct MenuButtonElements{
    std::string image;
    std::vector<MenuItemElements> items;
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

#endif
