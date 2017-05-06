#include "MenuButton.h"

MenuButton::MenuButton() {
	ButtonBackColor.r = 0x00;
	ButtonBackColor.g = 0x5F;
	ButtonBackColor.b = 0x6B;
	ButtonBackColor.a = 0xFF;
	MaskColor.r = 0x00;
	MaskColor.g = 0x8C;
	MaskColor.b = 0x9E;
	MaskColor.a = 0xFF;
	padding = 10;
	menustate = BUTTON_MENU_CLOSED;
	btype = BUTTON_TYPE_MENU;
	popMenu = new Menu;
}

int MenuButton::getX(){
    return ButtonBack.x;
}
int MenuButton::getY(){
    return ButtonBack.y;
}
int MenuButton::getW(){
    return ButtonBack.w;
}
int MenuButton::getH(){
    return ButtonBack.h;
}
//W and H should only be set by setProportional
void MenuButton::setW(int val){
}
void MenuButton::setH(int val){
}

bool MenuButton::Init(std::string ImagePath, SDL_Renderer *ren){
	if(!Button::Init(ImagePath, ren))
		return false;
	if(!popMenu->Init(ren))
		return false;
	Shadow = IMG_LoadTexture(ren, (SDL_GetBasePath() + (std::string)"Assets/shadow-small.png").c_str());
	if (Shadow == NULL){
		return false;
	}
	ButtonBack.x = x - padding;
	ButtonBack.y = y - padding;
	ButtonBack.w = w + 2*padding;
	ButtonBack.h = h + 2*padding;
	return true;
}

bool MenuButton::LoadItem(const std::string &ItemMessage, TTF_Font *font, SDL_Color color, SDL_Renderer *ren, int type, int action){
    return popMenu->LoadItem(ItemMessage, font, color, ren, type, action);
}
bool MenuButton::LoadItem(const std::string &ItemMessage, TTF_Font *font, SDL_Color color, SDL_Renderer *ren, int type, bool* quitref){
    return popMenu->LoadItem(ItemMessage, font, color, ren, type, quitref);
}
bool MenuButton::LoadItem(const std::string &ItemMessage, TTF_Font *font, SDL_Color color, SDL_Renderer *ren, int type, const char* AppPath, const char* AppParams){
    return popMenu->LoadItem(ItemMessage, font, color, ren, type, (char*)AppPath, (char*)AppParams);
}


//I assume this works...?
void MenuButton::SetProportionalSize(int width){
	int temph = (int)((float)ButtonBack.h/(float)ButtonBack.w * (float)width);
	ButtonBack.h = temph;
	ButtonBack.w = width;
	w = ButtonBack.w - 2*padding;
	h = ButtonBack.w - 2*padding;
}

bool MenuButton::Activate(){
	if(menustate == BUTTON_MENU_CLOSED) {
		menustate = BUTTON_MENU_OPEN;
		state = BUTTON_STATE_ACTIVE;
	}
	else if(menustate == BUTTON_MENU_OPEN){
		menustate = BUTTON_MENU_CLOSED;
		state = BUTTON_STATE_SELECTED;
	}
	return true;
}

bool MenuButton::ActivateMenuItem(){
	return popMenu->Activate();
}

void MenuButton::Render(SDL_Renderer *ren){
	ButtonBack.x = x - padding;
	ButtonBack.y = y - padding;
	ButtonBack.w = w + 2*padding;
	ButtonBack.h = h + 2*padding;

	//Draw Shadow
	SDL_Rect dst;
	dst.x = ButtonBack.x;
	dst.y = ButtonBack.y;
	dst.w = ButtonBack.w + 10;
	dst.h = ButtonBack.h + 10;
	SDL_RenderCopy(ren, Shadow, NULL, &dst);

	//Render color
	if(state == BUTTON_STATE_SELECTED)
		SDL_SetRenderDrawColor(ren, MaskColor.r, MaskColor.g, MaskColor.b, MaskColor.a);
	else
		SDL_SetRenderDrawColor(ren, ButtonBackColor.r, ButtonBackColor.g, ButtonBackColor.b, ButtonBackColor.a);
	SDL_RenderFillRect(ren, &ButtonBack);
	Button::Render(ren);

	//render menu
	if(menustate == BUTTON_MENU_OPEN){
		popMenu->setX(getX() - popMenu->getW() + ButtonBack.w);
		popMenu->setY(getY() - popMenu->getH());
		popMenu->Render(ren);
	}
}

void MenuButton::Cleanup() {
	popMenu->Cleanup();
	//SDL_free(popMenu);
	Button::Cleanup();
}
void MenuButton::MoveUp(){
    popMenu->MoveUp();
}
void MenuButton::MoveDown(){
    popMenu->MoveDown();
}
