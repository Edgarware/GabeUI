#pragma once

#include "Button.h"
#include "Menu.h"

enum {
	BUTTON_MENU_CLOSED=0,
	BUTTON_MENU_OPEN
};

class MenuButton :public Button {
private:
	Menu *popMenu;
	SDL_Rect ButtonBack;
	SDL_Color ButtonBackColor;
	int padding;
	int menustate;

public:
	MenuButton();
	int getX();
	int getY();
	int getW();
	int getH();
	void setW(int val);
	void setH(int val);
	bool Init(std::string ImagePath, SDL_Renderer *ren);
	bool LoadItem(const std::string &ItemMessage, TTF_Font *font, SDL_Color color, SDL_Renderer *ren, int type, int action);
	bool LoadItem(const std::string &ItemMessage, TTF_Font *font, SDL_Color color, SDL_Renderer *ren, int type, bool* quitref);
	bool LoadItem(const std::string &ItemMessage, TTF_Font *font, SDL_Color color, SDL_Renderer *ren, int type, const char* AppPath, const char* AppParams);
	void SetProportionalSize(int width);
	bool Activate();
	bool ActivateMenuItem();
	void Render(SDL_Renderer *ren);
	void Cleanup();
	void MoveUp();
	void MoveDown();
};
