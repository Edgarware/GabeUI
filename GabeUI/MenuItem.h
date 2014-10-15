#ifndef MENUITEM_H_
#define MENUITEM_H_

#include "Button.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <windows.h>
#include <shellapi.h>

enum {
	MENUITEM_SHUTDOWN_SHUTDOWN=0,
	MENUITEM_SHUTDOWN_REBOOT
};

enum {
	MENUITEM_TYPE_GENERIC=0,
	MENUITEM_TYPE_SHUTDOWN,
	MENUITEM_TYPE_APPLAUNCH
};

class MenuItem: public Button{
public:
	SDL_Rect Back;
	SDL_Color BackColor;
	int action;
	int padding;
	int midpadding;
	int type;
	std::string AppPath;
	std::string AppParams;

public:
	MenuItem();
	bool Init(const std::string &ItemMessage, TTF_Font *font, SDL_Color color, SDL_Renderer *ren, int type, int action);
	bool Init(const std::string &ItemMessage, TTF_Font *font, SDL_Color color, SDL_Renderer *ren, int type, std::string AppPath, std::string AppParams);
	bool Activate();
	void Render(SDL_Renderer *ren);
};

#endif