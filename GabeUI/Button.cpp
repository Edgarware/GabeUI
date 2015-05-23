#include "Button.h"

int getTextureWidth(SDL_Texture *tex) {
	int W;
	SDL_QueryTexture(tex, NULL, NULL, &W, NULL);
	return W;
}

int getTextureHeight(SDL_Texture *tex) {
	int H;
	SDL_QueryTexture(tex, NULL, NULL, NULL, &H);
	return H;
}

Button::Button(){
	x=0;
	y=0;
	w=0;
	h=0;
	bname = "";
	Image = NULL;
	Shadow = NULL;
	MaskColor.r = 0x00;
	MaskColor.g = 0xDF;
	MaskColor.b = 0xFC;
	MaskColor.a = 0x40;
	Mask.x = 0;
	Mask.y = 0;
	Mask.w = 0;
	Mask.h = 0;
	state = BUTTON_STATE_UNSELECTED;
	btype = BUTTON_TYPE_GENERIC;
}

Button::~Button(){

}

bool Button::Init(std::string ImagePath, SDL_Renderer *ren){
	//Load Texture
	Image = IMG_LoadTexture(ren, ImagePath.c_str());
	if (Image == NULL){
		return false;
	}
	w = getTextureWidth(Image);
	h = getTextureHeight(Image);

	return true;
}

//Set the height and keep the aspect ratio of the texture
void Button::SetProportionalSizeW(int width){
	h = (int)((float)getTextureHeight(Image)/(float)getTextureWidth(Image) * (float)width);
	w = width;
}

void Button::SetProportionalSizeH(int height){
	w = (int)((float)getTextureWidth(Image)/(float)getTextureHeight(Image) * (float)height);
	h = height;
}

bool Button::Activate(){
	//...
	return true;
}

void Button::Render(SDL_Renderer *ren){
	SDL_Rect dst;
	dst.x = x;
	dst.y = y;
	dst.w = w;
	dst.h = h;
	SDL_RenderCopy(ren, Image, NULL, &dst);
}

void Button::Cleanup(){
	SDL_DestroyTexture(Image);
	Image = NULL;
}
