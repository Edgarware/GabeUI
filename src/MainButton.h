#pragma once

#include "Button.h"

enum {
	BUTTON_ANIM_INACTIVE = 0,
	BUTTON_ANIM_UP,
	BUTTON_ANIM_DOWN,
	BUTTON_ANIM_ACTIVE_DOWN,
	BUTTON_ANIM_ACTIVE_UP
};

class MainButton : public Button {
private:
	char *application;
	char *args;
	uint32_t LastTime;
	int animState;
	int activeAnim;
	int TimeToWait; //number of (milliseconds??) to wait before running next frame
	int scale; //size to increase to (delta height = 2*scale), ALSO the number of frames
	SDL_Rect B_Size; //Boundaries for LARGE (selected) button
	SDL_Rect S_Size; //Boundaries for SMALL (unselected, default) button;
	int shadowOffset;
	int shadowPadding;

public:
	MainButton();
	void setApp(const char *app, const char *args);
    void setW(int val);
    void setH(int val);
	bool Init(std::string ImagePath, SDL_Renderer *ren);
	void SetXY(int X, int Y);
	void SetProportionalSizeW(int width);
	void SetProportionalSizeH(int height);
	bool Activate();
	void Render(SDL_Renderer *ren);
	void Cleanup();

private:
	void Animate();
};
