#pragma once

#include <string>
#include "OS.h"

/********
Button - Self enclosed button for creating interactable buttons
REFFERENCE CLASS - DO NOT DIRECTLY CALL
********/
enum
{
	BUTTON_TYPE_GENERIC=0,
	BUTTON_TYPE_MAIN,
	BUTTON_TYPE_MENU,
	BUTTON_TYPE_MENUITEM
};

enum
{
	BUTTON_STATE_INIT=0,
	BUTTON_STATE_UNSELECTED,
	BUTTON_STATE_SELECTED,
	BUTTON_STATE_ACTIVE
};


class Button{

protected:
	SDL_Texture* Image;
	SDL_Texture* Shadow;
	SDL_Rect Mask;
	SDL_Color MaskColor;
	int state;
	int x;
	int y;
	int w;
	int h;
	int btype;
	//std::string bname;

public:
	Button();
	virtual ~Button();

	virtual int getX();
	virtual int getY();
	virtual int getW();
	virtual int getH();
	virtual int getState();
	virtual int getType();
	virtual void setX(int val);
	virtual void setY(int val);
	virtual void setW(int val);
	virtual void setH(int val);
	virtual void setState(int val);

	virtual bool Init(std::string ImagePath, SDL_Renderer *ren);
	virtual void SetProportionalSizeW(int width);
	virtual void SetProportionalSizeH(int height);
	virtual bool Activate();
	virtual void Render(SDL_Renderer *ren);
	virtual void Cleanup();
};
