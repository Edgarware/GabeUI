#ifndef WIFIMANAGER_H_
#define WIFIMANAGER_H_

#include <windows.h>
#include <vector>
#include <objbase.h>
#include <wtypes.h>
#include <wlanapi.h>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include <string>
#include <iostream>

enum { //state of the button to bring up main panel
	WIFI_BUTTON_STATE_INIT = 0,
	WIFI_BUTTON_STATE_UNSELECTED,
	WIFI_BUTTON_STATE_SELECTED,
	WIFI_BUTTON_STATE_ACTIVE
};

class WifiManager{

public: //Wifi Logic vars
	std::vector<WLAN_AVAILABLE_NETWORK> NetList;
	HANDLE WifiHandle;
	PWLAN_INTERFACE_INFO NetInterface;
public: //GUI vars
	std::vector<SDL_Texture*> Bars;
	std::vector<SDL_Texture*> SSIDs;
	TTF_Font *textFont;
	SDL_Texture* ButtonImage;
	SDL_Texture* ButtonShadow;
	SDL_Rect ButtonBack;
	SDL_Color ButtonMaskColor;
	SDL_Color ButtonBackColor;
	SDL_Rect Back;
	SDL_Color BackColor;
	SDL_Color TextColor;
	int padding;
	int sidePadding;
	int selected;
	int state;
	int x, y;
	int width, height; //width/height of wifi panel (whole thing)
	int nwidth, nheight; //width/height of each individual 
	int bx, by; //x/y position of button to bring up main panel
	int bwidth, bheight; //width/height of button to bring up main panel
public:
	WifiManager();
	bool InitWifiInterface();
	bool InitGui(std::string ButtonIconPath, TTF_Font *font, SDL_Renderer *ren);
	void Render(SDL_Renderer *ren);
	bool Activate();
	void DeActivate();
	void Cleanup();
private:
	bool Rescan();
	bool RescanGui(SDL_Renderer *ren);
};

#endif