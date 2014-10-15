#ifndef MENU_H_
#define MENU_H_

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>


#include "MenuItem.h"
//I'm pretty happy with this finally

class Menu {
public: 
	//yo dog, i heard you like objects, so I made an object that contains an object that holds a list of objects
	std::vector<MenuItem> MenuItems;
	SDL_Texture *Shadow;
	int x;
	int y;
	int w;
	int h;
	int selected;
	
public:
	Menu();
	~Menu();
	bool Init(SDL_Renderer *ren);
	bool Activate();
	bool LoadItem(const std::string &ItemMessage, TTF_Font *font, SDL_Color color, SDL_Renderer *ren, int type, int action);
	bool LoadItem(const std::string &ItemMessage, TTF_Font *font, SDL_Color color, SDL_Renderer *ren, int type, std::string AppPath, std::string AppParams);
	void Render(SDL_Renderer *ren);
	void Cleanup();
	void MoveUp();
	void MoveDown();

};

#endif