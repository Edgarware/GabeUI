#include "MenuItem.h"

int getTextWidth(SDL_Texture *tex) {
	int W;
	SDL_QueryTexture(tex, NULL, NULL, &W, NULL);
	return W;
}

int getTextHeight(SDL_Texture *tex) {
	int H;
	SDL_QueryTexture(tex, NULL, NULL, NULL, &H);
	return H;
}

MenuItem::MenuItem(){
	MaskColor.r = 0x00;
	MaskColor.g = 0xB4;
	MaskColor.b = 0xCC;
	MaskColor.a = 0xFF;
	BackColor.r = 0x00;
	BackColor.g = 0x8C;
	BackColor.b = 0x9E;
	BackColor.a = 0xFF;
	action = 0;
	type = MENUITEM_TYPE_GENERIC;
	btype = BUTTON_TYPE_MENUITEM;
	AppPath = "";
	AppParams = "";
	padding = 10;
	midpadding = 5;
}

bool MenuItem::Init(const std::string &ItemMessage, TTF_Font *font, SDL_Color color, SDL_Renderer *ren, int Type, char* appPath, char* appParams){
	if(Type == MENUITEM_TYPE_APPLAUNCH){
		//We need to first render to a surface as that's what TTF_RenderText returns, then load that surface into a texture
		SDL_Surface *surf = TTF_RenderText_Blended(font, ItemMessage.c_str(), color);
		if (surf == NULL){
			return false;
		}
		Image = SDL_CreateTextureFromSurface(ren, surf);
		if (Image == NULL){
			SDL_FreeSurface(surf);
			return false;
		}
		//Clean up the surface
		SDL_FreeSurface(surf);
		//set w/h
		w = getTextWidth(Image) + 2*padding;
		h = getTextHeight(Image) + midpadding;
		//set vars
		type = Type;
		AppPath = appPath;
		AppParams = appParams;

		return true;
	}
	else //incorrect type
		return false;
}

bool MenuItem::Init(const std::string &ItemMessage, TTF_Font *font, SDL_Color color, SDL_Renderer *ren, int Type, int Action){
	if(Type == MENUITEM_TYPE_SHUTDOWN){
		//We need to first render to a surface as that's what TTF_RenderText returns, then load that surface into a texture
		SDL_Surface *surf = TTF_RenderText_Blended(font, ItemMessage.c_str(), color);
		if (surf == NULL){
			return false;
		}
		Image = SDL_CreateTextureFromSurface(ren, surf);
		if (Image == NULL){
			SDL_FreeSurface(surf);
			return false;
		}
		//Clean up the surface
		SDL_FreeSurface(surf);
		//set w/h
		w = getTextWidth(Image) + 2*padding;
		h = getTextHeight(Image) + midpadding;
		//set vars
		type = Type;
		action = Action;

		return true;
	}
	else //incorrect type
		return false;
}

bool MenuItem::Init(const std::string &ItemMessage, TTF_Font *font, SDL_Color color, SDL_Renderer *ren, int Type, bool *quit){
	if(Type == MENUITEM_TYPE_QUIT){
		//We need to first render to a surface as that's what TTF_RenderText returns, then load that surface into a texture
		SDL_Surface *surf = TTF_RenderText_Blended(font, ItemMessage.c_str(), color);
		if (surf == NULL){
			return false;
		}
		Image = SDL_CreateTextureFromSurface(ren, surf);
		if (Image == NULL){
			SDL_FreeSurface(surf);
			return false;
		}
		//Clean up the surface
		SDL_FreeSurface(surf);
		//set w/h
		w = getTextWidth(Image) + 2*padding;
		h = getTextHeight(Image) + midpadding;
		//set vars
		type = Type;
		quitref = quit;

		return true;
	}
	else //incorrect type
		return false;
}

bool MenuItem::Activate(){
	if(type == MENUITEM_TYPE_SHUTDOWN) {
		if(action == MENUITEM_SHUTDOWN_SHUTDOWN) {
			system("/sbin/halt");
            return true;
		}
		else if(action == MENUITEM_SHUTDOWN_REBOOT) {
			system("/sbin/reboot");
			return true;
		}
		else
			return false;
	}
	else if(type == MENUITEM_TYPE_APPLAUNCH) {
		system(AppPath);
		return true;
	}
	else if(type == MENUITEM_TYPE_QUIT){
	    if(quitref != NULL){
            *quitref = true;
            return true;
	    }
	    else
            return false;
	}
	//broken type
	else
		return false;
}

void MenuItem::Render(SDL_Renderer *ren){
	//Draw background / Mask
	Back.x = x;
	Back.y = y;
	Back.w = w;
	Back.h = h;

	if (state == BUTTON_STATE_SELECTED)
		SDL_SetRenderDrawColor(ren, MaskColor.r, MaskColor.g, MaskColor.b, MaskColor.a);
	else
		SDL_SetRenderDrawColor(ren, BackColor.r, BackColor.g, BackColor.b, BackColor.a);
	SDL_RenderFillRect(ren, &Back);

	//Draw text
	float temp;
	SDL_Rect dst2;
	temp = x + ((float)w/2) - ((float)getTextWidth(Image)/2);
	dst2.x = (int)temp;
	dst2.y = y + midpadding;
	dst2.w = getTextWidth(Image);
	dst2.h = getTextHeight(Image);
	SDL_RenderCopy(ren, Image, NULL, &dst2);
}
