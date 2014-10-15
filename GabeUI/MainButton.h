#ifndef MAINBUTTON_H_
#define MAINBUTTON_H_

#include <windows.h>
#include <shellapi.h>
#include "Button.h"

enum {
	BUTTON_ANIM_INACTIVE = 0,
	BUTTON_ANIM_UP,
	BUTTON_ANIM_DOWN
};

//TODO: Outline?

class MainButton :public Button {
public:
	std::string AppPath;
	std::string AppParams;
	Uint32 LastTime;
	int animState;
	int TimeToWait; //number of (milliseconds??) to wait before running next frame
	int scale; //size to increase to (delta height = 2*scale), ALSO the number of frames
	SDL_Rect B_Size; //Boundaries for LARGE (selected) button
	SDL_Rect S_Size; //Boundaries for SMALL (unselected, default) button;
	int shadowOffset;

public:
	MainButton();
	bool Init(std::string ImagePath, SDL_Renderer *ren);
	void SetProportionalSize(int width);
	bool Activate();
	void Render(SDL_Renderer *ren);
	void Cleanup();

private:
	void Animate();
};

#endif