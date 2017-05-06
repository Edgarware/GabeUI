#ifndef MENUITEM_H_
#define MENUITEM_H_

#include "Button.h"
#include <stdlib.h>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

enum {
	MENUITEM_SHUTDOWN_SHUTDOWN=0,
	MENUITEM_SHUTDOWN_REBOOT
};

enum {
	MENUITEM_TYPE_GENERIC=0,
	MENUITEM_TYPE_SHUTDOWN,
	MENUITEM_TYPE_APPLAUNCH,
	MENUITEM_TYPE_QUIT
};

class MenuItem: public Button{
public:
	SDL_Rect Back;
	SDL_Color BackColor;
	int action;
	int padding;
	int midpadding;
	int type;
	char* AppPath;
	char* AppParams;
	bool *quitref;


public:
	MenuItem();
	bool Init(const std::string &ItemMessage, TTF_Font *font, SDL_Color color, SDL_Renderer *ren, int type, int action);
	bool Init(const std::string &ItemMessage, TTF_Font *font, SDL_Color color, SDL_Renderer *ren, int type, char* AppPath, char* AppParams);
	bool Init(const std::string &ItemMessage, TTF_Font *font, SDL_Color color, SDL_Renderer *ren, int type, bool* quit);
	bool Activate();
	void Render(SDL_Renderer *ren);
};

#endif
