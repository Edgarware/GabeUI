#include "MainButton.h"

MainButton::MainButton(){
	btype = BUTTON_TYPE_MAIN;
	LastTime = 0;
	scale = 10;
	TimeToWait = 10;
	activeAnim = 10;
	shadowOffset = -10;
	shadowPadding = 40;
	animState = BUTTON_ANIM_INACTIVE;
	MaskColor.r = 0x00;
	MaskColor.g = 0x00;
	MaskColor.b = 0x00;
	MaskColor.a = 0xA0;
	S_Size.x = 0;
	S_Size.y = 0;
	S_Size.w = 0;
	S_Size.h = 0;
	B_Size.x = 0;
	B_Size.y = 0;
	B_Size.w = 0;
	B_Size.h = 0;
}

bool MainButton::Init(std::string ImagePath, SDL_Renderer *ren){
	//Load Texture
	Button::Init(ImagePath, ren);
	Shadow = IMG_LoadTexture(ren, (SDL_GetBasePath() + (std::string)"Assets\\shadow-small.png").c_str());
	if (Shadow == nullptr){
		return false;
	}
	//Set small button size
	S_Size.x = x;
	S_Size.y = y;
	S_Size.w = w;
	S_Size.h = h;

	//Calculate large button size
	B_Size.x = x - 2*scale;
	B_Size.y = y - scale;
	B_Size.w = w + 4*scale;
	B_Size.h = h + 2*scale;

	return true;
}

void MainButton::SetXY(int X, int Y){
	if (animState == BUTTON_ANIM_INACTIVE){
		x = X;
		y = Y;
		S_Size.x = x;
		S_Size.y = y;
		B_Size.x = x - 2 * scale;
		B_Size.y = y - scale;
	}
}

void MainButton::SetProportionalSize(int width){
	if (animState == BUTTON_ANIM_INACTIVE){
		Button::SetProportionalSize(width);
		//Set new animation references
		//Set small button size
		S_Size.w = w;
		S_Size.h = h;

		//Calculate large button size
		B_Size.w = w + 4 * scale;
		B_Size.h = h + 2 * scale;
	}
}

bool MainButton::Activate(){
	DWORD iRes = (DWORD)ShellExecute(NULL, "open", AppPath.c_str(), AppParams.c_str(), NULL, SW_NORMAL);
	if(iRes < 32)
		return false;
	//animState = BUTTON_ANIM_ACTIVE_DOWN;
	return true;
}

void MainButton::Render(SDL_Renderer *ren){
	//Run Animation logic
	if (scale != 0){
		Animate();
	}
	else{
		if (state == BUTTON_STATE_SELECTED){
			MaskColor.a = 0x00;
		}
		else{
			MaskColor.a = 0xA0;
		}
	}

	//Calculate Mask size after Animation
	Mask.x = x;
	Mask.y = y;
	Mask.w = w;
	Mask.h = h;
	
	//Draw Shadow
	SDL_Rect dst;
	dst.x = S_Size.x + shadowOffset;
	dst.y = S_Size.y + shadowOffset;
	dst.w = S_Size.w + shadowPadding;
	dst.h = S_Size.h + shadowPadding;
	SDL_RenderCopy(ren, Shadow, NULL, &dst);
	
	//Draw Image
	Button::Render(ren);

	//Draw Mask
	//Mask is toggled on/off via Animate(), changes alpha
	SDL_SetRenderDrawColor(ren, MaskColor.r, MaskColor.g, MaskColor.b, MaskColor.a);
	SDL_RenderFillRect(ren, &Mask);
}

void MainButton::Cleanup(){
	Button::Cleanup();
	SDL_DestroyTexture(Shadow);
	Shadow = NULL;
}

void MainButton::Animate() {
	//START ANIMATING SMALL TO LARGE
	if(state == BUTTON_STATE_SELECTED && animState == BUTTON_ANIM_INACTIVE && x == S_Size.x && y == S_Size.y && w == S_Size.w && h == S_Size.h ) {
		animState = BUTTON_ANIM_UP;
		LastTime = SDL_GetTicks();
		//we wont do any anim until next frame, but thats like a couple milliseconds from now at most, so whatevs
	}
	//START ANIMATING LARGE TO SMALL
	else if(state == BUTTON_STATE_UNSELECTED && animState == BUTTON_ANIM_INACTIVE && x == B_Size.x && y == B_Size.y && w == B_Size.w && h == B_Size.h ) {
		animState = BUTTON_ANIM_DOWN;
		LastTime = SDL_GetTicks();
	}
	//ANIMATE SMALL TO LARGE
	else if(animState == BUTTON_ANIM_UP) {
		if(LastTime + TimeToWait > SDL_GetTicks()) {}
		else {
			LastTime = SDL_GetTicks();
			x -= 2;
			y--;
			w += 4; //this was x2, but looks better with x4? itsamystery
			h += 2; //since h is twice padding, increase by 2
			shadowOffset += 5; //Animate Shadow
			if(MaskColor.a != 0x00) //Animate alpha mask off
				MaskColor.a -= 0x10;
		}

		if(x == B_Size.x && y == B_Size.y && w == B_Size.w && h == B_Size.h) {
			//WE DONE, STOP THIS MADNESS
			animState = BUTTON_ANIM_INACTIVE;
		}
	}
	//ANIMATE LARGE TO SMALL
	else if(animState == BUTTON_ANIM_DOWN) {
		if(LastTime + TimeToWait > SDL_GetTicks()) {}
		else {
			LastTime = SDL_GetTicks();
			x +=2;
			y++;
			w -= 4; //this was x2, but looks better with x4? itsamystery
			h -= 2; //since h is twice padding, increase by 2
			shadowOffset -= 5; //Animate Shadow
			if(MaskColor.a != 0xFF) // Animate alpha mask on
				MaskColor.a += 0x10;
		}

		if(x == S_Size.x && y == S_Size.y && w == S_Size.w && h == S_Size.h) {
			//WE DONE, STOP THIS MADNESS
			animState = BUTTON_ANIM_INACTIVE;
		}
	}
}