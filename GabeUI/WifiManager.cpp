#include "WifiManager.h"

int getWidth(SDL_Texture *tex) {
	int W;
	SDL_QueryTexture(tex, NULL, NULL, &W, NULL);
	return W;
}

int getHeight(SDL_Texture *tex) {
	int H;
	SDL_QueryTexture(tex, NULL, NULL, NULL, &H);
	return H;
}

WifiManager::WifiManager(){
	ButtonImage = NULL;
	ButtonShadow = NULL;
	ButtonBackColor.r = 0x00;
	ButtonBackColor.g = 0x5F;
	ButtonBackColor.b = 0x6B;
	ButtonBackColor.a = 0xFF;
	ButtonMaskColor.r = 0x00;
	ButtonMaskColor.g = 0x8C;
	ButtonMaskColor.b = 0x9E;
	ButtonMaskColor.a = 0xFF;
	BackColor.r = 0x20;
	BackColor.g = 0x20;
	BackColor.b = 0x20;
	BackColor.a = 0xFF;
	TextColor.r = 0xFF;
	TextColor.g = 0xFF;
	TextColor.b = 0xFF;
	TextColor.a = 0xFF;
	padding = 10;
	sidePadding = 25;
	selected = 0;
	state = WIFI_BUTTON_STATE_UNSELECTED;
	x = 0;
	y = 0;
	width = 0;
	height = 0;
	nwidth = 0;
	nheight = 0;
	bx = 0;
	by = 0;
	bwidth = 0;
	bheight = 0;
	width = 0;
	height = 0;
	WifiHandle = NULL;
}

bool WifiManager::InitWifiInterface(){
	DWORD clientVersion = 0;

	//Open handles & get wireless interface
	if (WlanOpenHandle(WLAN_API_VERSION, NULL, &clientVersion, &WifiHandle) != ERROR_SUCCESS)
		return false;
	if (WLAN_API_VERSION_MAJOR(clientVersion) < WLAN_API_VERSION_MAJOR(WLAN_API_VERSION_2_0)) //Ew, gross. Windows XP.
		return false;
	PWLAN_INTERFACE_INFO_LIST interfaceList = NULL;
	if (WlanEnumInterfaces(WifiHandle, NULL, &interfaceList) != ERROR_SUCCESS)
		return false;
	for (int i = 0; i < (int)interfaceList->dwNumberOfItems; i++) { //Save the first not-broken interface as the only one because LAZY
		if (interfaceList->InterfaceInfo[i].isState != wlan_interface_state_not_ready && interfaceList->InterfaceInfo[i].isState != wlan_interface_state_ad_hoc_network_formed && NetInterface == NULL)
		{
			NetInterface = (WLAN_INTERFACE_INFO *)&interfaceList->InterfaceInfo[i];
			break;
		}
	}

	WifiManager::Rescan();
		
	//Cleanup!
	//if (interfaceList != NULL) {
	//	WlanFreeMemory(interfaceList);
	//	interfaceList = NULL;
	//}
	return true;
}

bool WifiManager::Rescan(){
	while (!NetList.empty()){ //empty out the vector
		NetList.erase(NetList.begin());
	}

	DWORD dwResult = 0;
	
	//Get list of nearby connections
	PWLAN_AVAILABLE_NETWORK_LIST aNetList = NULL;
	if ((dwResult = WlanGetAvailableNetworkList(WifiHandle, &NetInterface->InterfaceGuid, 0, NULL, &aNetList)) != ERROR_SUCCESS) {
		char *err;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwResult, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&err, 0, NULL);
		std::cout << err << std::endl;
		return false;
	}

	//DATA MANAGEMENT!!
	for (int i = 0; i < (int)aNetList->dwNumberOfItems; i++) { //throw this mess in a vector so I can actually use it
		if (aNetList->Network[i].bNetworkConnectable && aNetList->Network[i].dot11Ssid.uSSIDLength != 0)
			NetList.push_back(aNetList->Network[i]);
	}

	//God damn you hidden networks
	for (unsigned int i = 0; i < NetList.size(); i++){
		if (NetList[i].dot11Ssid.ucSSID[0] == 0) //Yeah, if your SSID starts with an empty char this DOES fuck up. Maybe dont do that.
			NetList.erase(NetList.begin() + i);
	}


	int starting = 0;
	WLAN_AVAILABLE_NETWORK tempNet;
	for (unsigned int i = 0; i < NetList.size(); i++){ //set the connected network (if applicable) to the front of the list
		if (NetList[i].dwFlags & WLAN_AVAILABLE_NETWORK_CONNECTED) {
			tempNet = NetList[starting];
			NetList[starting] = NetList[i];
			NetList[i] = tempNet;
			starting++;
		}
	}

	for (unsigned int i = starting; i < NetList.size(); i++){ //sort the rest of these poor saps by signal strength
		for (unsigned int k = i + 1; k < NetList.size(); k++){
			if (NetList[i].wlanSignalQuality < NetList[k].wlanSignalQuality){
				tempNet = NetList[k];
				NetList[k] = NetList[i];
				NetList[i] = tempNet;
				starting++;
			}
		}
	}

	for (unsigned int i = 0; i < NetList.size(); i++){ //Erase Dupelicate elements (TODO: Actual reason to delete them?)
		std::string temp(NetList[i].dot11Ssid.ucSSID, NetList[i].dot11Ssid.ucSSID + NetList[i].dot11Ssid.uSSIDLength);
		for (unsigned int k = i + 1; k < NetList.size(); k++){
			std::string temp2(NetList[k].dot11Ssid.ucSSID, NetList[k].dot11Ssid.ucSSID + NetList[k].dot11Ssid.uSSIDLength);
			if (temp == temp2){
				if (!(NetList[k].dwFlags & WLAN_AVAILABLE_NETWORK_HAS_PROFILE)){
					NetList.erase(NetList.begin() + k);
				}
			}
		}
	}

	//Cleanup!
	//if (aNetList != NULL) {
	//	WlanFreeMemory(aNetList);
	//	aNetList = NULL;
	//}
	return true;
}

bool WifiManager::InitGui(std::string ButtonIconPath, TTF_Font *font, SDL_Renderer *ren){
	//Load all those Textures
	textFont = font;
	ButtonImage = IMG_LoadTexture(ren, ButtonIconPath.c_str());
	if (ButtonImage == nullptr){
		return false;
	}
	ButtonShadow = IMG_LoadTexture(ren, (SDL_GetBasePath() + (std::string)"Assets\\shadow-small.png").c_str());
	if (ButtonShadow == nullptr){
		return false;
	}
	//load all them Icons
	SDL_Texture* temp = IMG_LoadTexture(ren, (SDL_GetBasePath() + (std::string)"Assets\\Wifi\\0.png").c_str());
	if (temp == nullptr){
		return false;
	}
	Bars.push_back(temp);
	temp = IMG_LoadTexture(ren, (SDL_GetBasePath() + (std::string)"Assets\\Wifi\\1.png").c_str());
	if (temp == nullptr){
		return false;
	}
	Bars.push_back(temp);
	temp = IMG_LoadTexture(ren, (SDL_GetBasePath() + (std::string)"Assets\\Wifi\\2.png").c_str());
	if (temp == nullptr){
		return false;
	}
	Bars.push_back(temp);
	temp = IMG_LoadTexture(ren, (SDL_GetBasePath() + (std::string)"Assets\\Wifi\\3.png").c_str());
	if (temp == nullptr){
		return false;
	}
	Bars.push_back(temp);
	temp = IMG_LoadTexture(ren, (SDL_GetBasePath() + (std::string)"Assets\\Wifi\\4.png").c_str());
	if (temp == nullptr){
		return false;
	}
	Bars.push_back(temp);
	temp = IMG_LoadTexture(ren, (SDL_GetBasePath() + (std::string)"Assets\\Wifi\\5.png").c_str());
	if (temp == nullptr){
		return false;
	}
	Bars.push_back(temp);

	//Calculate all themthere widths
	bwidth = getWidth(ButtonImage);
	bheight = getHeight(ButtonImage);
	ButtonBack.x = bx - padding;
	ButtonBack.y = by - padding;
	ButtonBack.w = bwidth + 2 * padding;
	ButtonBack.h = bheight + 2 * padding;
	return true;
}

bool WifiManager::RescanGui(SDL_Renderer *ren){
	if (SSIDs.size() != NetList.size()) {
		if (SSIDs.size() != 0) {
			while (!SSIDs.empty()){ //empty out the vector
				SDL_free((*SSIDs.begin()));
				SSIDs.erase(SSIDs.begin());
			}
		}
		SDL_Surface *tempSurf = NULL;
		SDL_Texture *tempText = NULL;
		//Save the SSIDs as Texture Data
		for (unsigned int i = 0; i < NetList.size(); i++){
			std::string tempString(NetList[i].dot11Ssid.ucSSID, NetList[i].dot11Ssid.ucSSID + NetList[i].dot11Ssid.uSSIDLength);
			tempSurf = TTF_RenderText_Blended(textFont, tempString.c_str(), TextColor);
			if (tempSurf == nullptr){
				return false;
			}
			tempText = SDL_CreateTextureFromSurface(ren, tempSurf);
			if (tempText == nullptr){
				SDL_FreeSurface(tempSurf);
				return false;
			}
			SSIDs.push_back(tempText);
		}
		SDL_FreeSurface(tempSurf);
		return true;
	}
	else
		return true;
}

void WifiManager::Render(SDL_Renderer *ren){

	WifiManager::RescanGui(ren);

	ButtonBack.x = bx - padding;
	ButtonBack.y = by - padding;
	ButtonBack.w = bwidth + 2 * padding;
	ButtonBack.h = bheight + 2 * padding;

	//Draw Shadow
	SDL_Rect dst;
	dst.x = ButtonBack.x;
	dst.y = ButtonBack.y;
	dst.w = ButtonBack.w + 10;
	dst.h = ButtonBack.h + 10;
	SDL_RenderCopy(ren, ButtonShadow, NULL, &dst);

	//Render Mask
	if (state == WIFI_BUTTON_STATE_SELECTED)
		SDL_SetRenderDrawColor(ren, ButtonMaskColor.r, ButtonMaskColor.g, ButtonMaskColor.b, ButtonMaskColor.a);
	else
		SDL_SetRenderDrawColor(ren, ButtonBackColor.r, ButtonBackColor.g, ButtonBackColor.b, ButtonBackColor.a);
	SDL_RenderFillRect(ren, &ButtonBack);

	//Render Button Icon
	dst.x = bx;
	dst.y = by;
	dst.w = bwidth;
	dst.h = bheight;
	SDL_RenderCopy(ren, ButtonImage, NULL, &dst);

	//Render Main Panel
	if (state == WIFI_BUTTON_STATE_ACTIVE){
		Back.x = x;
		Back.y = y;
		Back.w = width;
		Back.h = height;
		SDL_SetRenderDrawColor(ren, BackColor.r, BackColor.g, BackColor.b, BackColor.a);
		SDL_RenderFillRect(ren, &Back);

		//Render Networks
		int tempx = x;
		int tempy = y;
		nwidth = height;
		nheight = height;
		SDL_Rect dst2;
		for (unsigned int i = 0; i < NetList.size(); i++){
			//Selected Mask
			dst.x = tempx;
			dst.y = tempy;
			dst.w = nwidth;
			dst.h = nheight;
			SDL_SetRenderDrawColor(ren, 0x80, 0x80, 0x80, 0xff);
			if (i == selected)
				SDL_RenderFillRect(ren, &dst);

			//Draw Bars
			dst.x = tempx + nwidth/4;
			dst.y = tempy;
			dst.w = nwidth/2;
			dst.h = nheight/2;
			if (NetList[i].wlanSignalQuality <= 10)
				SDL_RenderCopy(ren, Bars[0], NULL, &dst);
			else if (NetList[i].wlanSignalQuality > 10 && NetList[i].wlanSignalQuality <= 25)
				SDL_RenderCopy(ren, Bars[1], NULL, &dst);
			else if (NetList[i].wlanSignalQuality > 25 && NetList[i].wlanSignalQuality <= 45)
				SDL_RenderCopy(ren, Bars[2], NULL, &dst);
			else if (NetList[i].wlanSignalQuality > 45 && NetList[i].wlanSignalQuality <= 65)
				SDL_RenderCopy(ren, Bars[3], NULL, &dst);
			else if (NetList[i].wlanSignalQuality > 65 && NetList[i].wlanSignalQuality <= 80)
				SDL_RenderCopy(ren, Bars[4], NULL, &dst);
			else if (NetList[i].wlanSignalQuality > 80)
				SDL_RenderCopy(ren, Bars[5], NULL, &dst);

			//Draw Text
			if (i < SSIDs.size()){ //man shit is fucked otherwise
				dst2.x = tempx + sidePadding;
				dst2.y = tempy + (nwidth / 2) + sidePadding;
				dst2.w = getWidth(SSIDs[i]);
				dst2.h = getHeight(SSIDs[i]);
				SDL_RenderCopy(ren, SSIDs[i], NULL, &dst2);
			}
			tempx += nwidth;
		}
	}
}

bool WifiManager::Activate(){
	if (state == WIFI_BUTTON_STATE_SELECTED) {
		state = WIFI_BUTTON_STATE_ACTIVE;
		WifiManager::Rescan(); //Rescan networks when we reopen the menu
	}
	return true;
}

void WifiManager::DeActivate(){
	if (state == WIFI_BUTTON_STATE_ACTIVE)
		state = WIFI_BUTTON_STATE_SELECTED;
}

void WifiManager::Cleanup(){
	//Some voodoo bullshit going on here
	/*for (std::vector<WLAN_AVAILABLE_NETWORK>::iterator MenuItemsIt = NetList.begin(); MenuItemsIt != NetList.end(); MenuItemsIt++){
	}

	if (NetInterface != NULL) {
		WlanFreeMemory(NetInterface);
	}*/

	SDL_DestroyTexture(ButtonImage);
	ButtonImage = NULL;
	SDL_DestroyTexture(ButtonShadow);
	ButtonShadow = NULL;
	
}
