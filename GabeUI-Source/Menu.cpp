#include "Menu.h"

Menu::Menu() {
	x = 0;
	y = 0;
	w = 0;
	h = 0;
	selected = 0;
}

Menu::~Menu(){
}

int Menu::getX(){
    return x;
}
int Menu::getY(){
    return y;
}
int Menu::getW(){
    return w;
}
int Menu::getH(){
    return h;
}
void Menu::setX(int val){
    x = val;
}
void Menu::setY(int val){
    y = val;
}
void Menu::setW(int val){
    w = val;
}
void Menu::setH(int val){
    h = val;
}

bool Menu::Init(SDL_Renderer *ren){
	Shadow = IMG_LoadTexture(ren, (SDL_GetBasePath() + (std::string)"Assets/shadow-small.png").c_str());
	if (Shadow == NULL){
		return false;
	}
	return true;
}

bool Menu::Activate(){

	for(std::vector<MenuItem>::iterator MenuItemsIt = MenuItems.begin(); MenuItemsIt != MenuItems.end(); MenuItemsIt++){
		if(std::distance(MenuItems.begin(), MenuItemsIt) == selected){
		     return (*MenuItemsIt).Activate();
		}
	}
	//nothing selected?
	return false;
}

bool Menu::LoadItem(const std::string &ItemMessage, TTF_Font *font, SDL_Color color, SDL_Renderer *ren, int type, int action){
	MenuItem temp;
	if(!temp.Init(ItemMessage, font, color, ren, type, action))
		return false;
	if(temp.getW() > w) {
		w = temp.getW();
	}
	h += temp.getH();
	if(MenuItems.size() == 0)
		temp.setState(BUTTON_STATE_SELECTED);
	else
		temp.setState(BUTTON_STATE_UNSELECTED);
	MenuItems.push_back(temp);
	return true;
}

bool Menu::LoadItem(const std::string &ItemMessage, TTF_Font *font, SDL_Color color, SDL_Renderer *ren, int type, bool *quitref){
	MenuItem temp;
	if(!temp.Init(ItemMessage, font, color, ren, type, quitref))
		return false;
	if(temp.getW() > w) {
		w = temp.getW();
	}
	h += temp.getH();
	if(MenuItems.size() == 0)
		temp.setState(BUTTON_STATE_SELECTED);
	else
		temp.setState(BUTTON_STATE_UNSELECTED);
	MenuItems.push_back(temp);
	return true;
}

bool Menu::LoadItem(const std::string &ItemMessage, TTF_Font *font, SDL_Color color, SDL_Renderer *ren, int type, char* AppPath, char* AppParams){
	MenuItem temp;
	if(!temp.Init(ItemMessage, font, color, ren, type, AppPath, AppParams))
		return false;
	if(temp.getW() > w) {
		w = temp.getW();
	}
	h += temp.getH();
	if(MenuItems.size() == 0)
		temp.setState(BUTTON_STATE_SELECTED);
	else
		temp.setState(BUTTON_STATE_UNSELECTED);
	MenuItems.push_back(temp);
	return true;
}

void Menu::Render(SDL_Renderer *ren){
	//Draw Shadow
	SDL_Rect dst;
	dst.x = x;
	dst.y = y;
	if(w != 0)
		dst.w = w + 20;
	if(h != 0)
		dst.h = h + 20;
	SDL_RenderCopy(ren, Shadow, NULL, &dst);

	//Loop through items
	int curY = 0;
	for(std::vector<MenuItem>::iterator MenuItemsIt = MenuItems.begin(); MenuItemsIt != MenuItems.end(); MenuItemsIt++){
		(*MenuItemsIt).setX(x);
		(*MenuItemsIt).setY(y + curY);
		(*MenuItemsIt).setW(w);

		(*MenuItemsIt).Render(ren);

		curY += (*MenuItemsIt).getH();
	}
}

void Menu::Cleanup(){
	for(std::vector<MenuItem>::iterator MenuItemsIt = MenuItems.begin(); MenuItemsIt != MenuItems.end(); MenuItemsIt++){
		(*MenuItemsIt).Cleanup();
	}
	SDL_DestroyTexture(Shadow);
	Shadow = NULL;
}

void Menu::MoveUp(){
	if(selected == 0 || MenuItems.size() == 0) {}
	else {
		MenuItems[selected].setState(BUTTON_STATE_UNSELECTED);
		selected--;
		MenuItems[selected].setState(BUTTON_STATE_SELECTED);
	}
}

void Menu::MoveDown(){
	if(selected == MenuItems.size()-1 || MenuItems.size() == 0) {}
	else {
		MenuItems[selected].setState(BUTTON_STATE_UNSELECTED);
		selected++;
		MenuItems[selected].setState(BUTTON_STATE_SELECTED);
	}
}
