#ifndef MENUBUTTON_H_
#define MENUBUTTON_H_

#include "Button.h"
#include "Menu.h"

enum {
	BUTTON_MENU_CLOSED=0,
	BUTTON_MENU_OPEN
};

/* Special Notes:
 *  Dont externally call this objects x, y, w, or h components! They're totally incorrect!
 *  If needed, call the button background's values
 *  EG: ExButton.ButtonBack.x
 */

class MenuButton :public Button {
public:
	Menu *popMenu;
	SDL_Rect ButtonBack;
	SDL_Color ButtonBackColor;
	int padding;
	int menustate;

public:
	MenuButton();
	bool Init(std::string ImagePath, SDL_Renderer *ren);
	void SetProportionalSize(int width);
	bool Activate();
	bool ActivateMenuItem();
	void Render(SDL_Renderer *ren);
	void Cleanup();
};

#endif