#include "App.h"

#define JOY_BUFFER 10000
#define JOY_TIME 200
#define DEBUG 0 //flag to run game in windowed mode. used in debug mode to see console
#define SPECIAL 0

/* COLOR MAP SO I DONT HAVE TO REMEMBER
 * #343438 #005F6B #008C9E #00B4CC #00DFFC
*/

//FUNCTIONS.H
void logSDLError(std::ostream &os, const std::string &msg){
	os << "error: " << msg << " " << SDL_GetError() << std::endl;
}

void logSDLMessage(std::ostream &os, const std::string &msg){
	os << msg << std::endl;
}

//CLEANUP.H
void cleanup(SDL_Window *win){
	SDL_DestroyWindow(win);
	win = NULL;
}
void cleanup(SDL_Renderer *ren){
	SDL_DestroyRenderer(ren);
	ren = NULL;
}
void cleanup(SDL_Texture *tex){
	SDL_DestroyTexture(tex);
	tex = NULL;
}
void cleanup(SDL_Surface *surf){
	SDL_FreeSurface(surf);
	surf = NULL;
}
void cleanup(TTF_Font *font){
	TTF_CloseFont(font);
	font = NULL;
}
template<typename A, typename B, typename C>
void cleanup(A *a, B *b, C *c){
	cleanup(a);
	cleanup(b);
	cleanup(c);
}

//should probably init vars but EH
App::App(){}

//Main program -----------------------------------------------------------------------------------------------------------------
int App::Main(int argc, char** argv){
	//INIT ALL THE THINGS
	//Make sure we have permissions for Shutdown/Restart
	bool ShutdownPossible = false;
	HANDLE hToken; 
	TOKEN_PRIVILEGES tkp; 
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
		LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid); 
		tkp.PrivilegeCount = 1;  // one privilege to set    
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
		AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0); 
		if (GetLastError() == ERROR_SUCCESS) {
			ShutdownPossible = true;
		}
		else {
			logSDLError(std::cout, "ShutdownPrivilege");
		}
		CloseHandle(hToken);
	}
	else {
		logSDLError(std::cout, "ShutdownPrivilege");
	}

	//Init SDL and Components
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
		logSDLError(std::cout, "SDL_Init");
		return 1;
	}
	if (TTF_Init() != 0){
		logSDLError(std::cout, "TTF_Init");
		return 1;
	}

	//Get window size & make that mess
	SDL_Rect displaySize;
	if(DEBUG == 0) {
		if (SDL_GetDisplayBounds(0, &displaySize) != 0) {
			logSDLError(std::cout, "CreateWindow");
			TTF_Quit();
			SDL_Quit();
			return 2;
		}
	}
	else {
		displaySize.x = SDL_WINDOWPOS_CENTERED;
		displaySize.y = SDL_WINDOWPOS_CENTERED;
		displaySize.w = 1024;
		displaySize.h = 576;
	}
	if(DEBUG == 0)
		window = SDL_CreateWindow("GabeUI", displaySize.x, displaySize.y, displaySize.w, displaySize.h, SDL_WINDOW_BORDERLESS); 
	else
		window = SDL_CreateWindow("GabeUI", displaySize.x, displaySize.y, displaySize.w, displaySize.h, SDL_WINDOW_INPUT_GRABBED); //we dont support resizable anymore OH WELL
	if (window == nullptr){
		logSDLError(std::cout, "CreateWindow");
		TTF_Quit();
		SDL_Quit();
		return 2;
	}

	//Make a renderer and set it's properties
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == nullptr){
		logSDLError(std::cout, "CreateRenderer");
		cleanup(window);
		TTF_Quit();
		SDL_Quit();
		return 3;
	}
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND); //Handle Transparency
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best"); //How we Scale
	SDL_ShowCursor(0); //We dont need no stinkin' mouse

	//Load Font
	SDL_Color textColor = {0xFF, 0xFF, 0xFF, 0xFF};
	font = TTF_OpenFont((SDL_GetBasePath() + (std::string)"Assets\\calibri.ttf").c_str(), 42);
	if (font == nullptr){
		logSDLError(std::cout, "TTF_OpenFont");
		cleanup(window);
		SDL_Quit();
		return 3;
	}
	TTF_SetFontHinting(font, TTF_HINTING_LIGHT); //Makes it prettier? SDL suggested it

	//Load Controller if applicable
	controller = NULL;
	scanForController();
	
	//Load Startup Items!
	DWORD iRes = (DWORD)ShellExecute(NULL, "open", "C:\\Program Files (x86)\\Plex\\Plex Media Server\\Plex Media Server.exe", "", NULL, SW_HIDE);
	if(iRes < 32)
		logSDLError(std::cout, "StartupItemLaunch");

	iRes = (DWORD)ShellExecute(NULL, "open", "C:\\Users\\GabeN\\Documents\\Utilities\\JoytoKey\\JoyToKey.exe", "", NULL, SW_HIDE);
	if(iRes < 32)
		logSDLError(std::cout, "StartupItemLaunch");

	//Init Buttons
	if (SPECIAL == 1){
		DB = new MainButton;
		if (!DB->Init((SDL_GetBasePath() + (std::string)"Assets\\DB_size.png"), renderer)) {
			logSDLError(std::cout, "CreateButton");
		}
		DB->AppPath = "C:\\Program Files (x86)\\Mozilla Firefox\\Firefox.exe";
		DB->AppParams = "-url www.twitch.tv/desertbus";
		DB->bname = "desertbus";
		DB->scale = 0;
		DB->shadowOffset = 0;
		DB->shadowPadding = 0;
		ButtonList.push_back(DB);
	}

	Steam = new MainButton;
	if(!Steam->Init((SDL_GetBasePath() + (std::string)"Assets\\steam.jpg"),renderer)) {
		logSDLError(std::cout, "CreateButton");
	}
	Steam->AppPath = "C:\\Program Files (x86)\\steam\\steam.exe";
	Steam->AppParams = "-start steam://open/bigpicture";
	Steam->bname = "steam";
	ButtonList.push_back(Steam);

	Plex = new MainButton;
	if(!Plex->Init((SDL_GetBasePath() + (std::string)"Assets\\plex.png"),renderer)) {
		logSDLError(std::cout, "CreateButton");
	}
	Plex->AppPath = "C:\\Program Files (x86)\\Plex Home Theater\\Plex Home Theater.exe";
	Plex->AppParams = "";
	Plex->bname = "plex";
	ButtonList.push_back(Plex);
	//MENU BUTTONS
	Options = new MenuButton;
	if(!Options->Init((SDL_GetBasePath() + (std::string)"Assets\\settings.png"),renderer)) {
		logSDLError(std::cout, "CreateButton");
	}
	Options->bname = "options";
	if(!Options->popMenu->LoadItem("Nvidia Settings", font, textColor, renderer, MENUITEM_TYPE_APPLAUNCH, "C:\\Program Files (x86)\\NVIDIA Corporation\\NVIDIA GeForce Experience\\GFExperience.exe",""))
		logSDLError(std::cout, "CreateMenuItem");
	if(!Options->popMenu->LoadItem("Plex Server Settings", font, textColor, renderer, MENUITEM_TYPE_APPLAUNCH, "C:\\Program Files (x86)\\Mozilla Firefox\\Firefox.exe","localhost:32400/web"))
		logSDLError(std::cout, "CreateMenuItem");
	ButtonList.push_back(Options);

	Exit = new MenuButton;
	if(!Exit->Init((SDL_GetBasePath() + (std::string)"Assets\\exit.png"),renderer)) {
		logSDLError(std::cout, "CreateButton");
	}
	Exit->bname = "exit";
	if(!Exit->popMenu->LoadItem("Shutdown", font, textColor, renderer, MENUITEM_TYPE_SHUTDOWN, MENUITEM_SHUTDOWN_SHUTDOWN))
		logSDLError(std::cout, "CreateMenuItem");
	if(!Exit->popMenu->LoadItem("Restart", font, textColor, renderer, MENUITEM_TYPE_SHUTDOWN, MENUITEM_SHUTDOWN_REBOOT))
		logSDLError(std::cout, "CreateMenuItem");
	if(!Exit->popMenu->LoadItem("Go To Desktop", font, textColor, renderer, MENUITEM_TYPE_APPLAUNCH, "C:\\Windows\\explorer.exe",""))
		logSDLError(std::cout, "CreateMenuItem");
	ButtonList.push_back(Exit);

	//WifiConfig
	Wireless = new WifiManager;
	if (Wireless->InitWifiInterface() == false)
		wifiOn = false;
	else
		Wireless->InitGui((SDL_GetBasePath() + (std::string)"Assets\\wifi.png"), font, renderer);
	

	//POSITION BUTTONS
	int wW, wH;
	int sidePadding;
	//Calculate the Size and Position of all buttons
	SDL_GetWindowSize(window, &wW, &wH);
	//side padding is 10% from the edges
	float temp = (float)(wH) * 0.10f; //fun with floats
	sidePadding = (int)(temp);
	
	//main buttons
	temp = ((float)(wW)/2) - ((float)(sidePadding)/2) - (float)sidePadding;
	
	Steam->SetXY(sidePadding, sidePadding);
	Steam->SetProportionalSize((int)temp);
	
	Plex->SetXY((wW / 2) + (sidePadding / 2), sidePadding);
	Plex->SetProportionalSize((int)temp);
	
	//menu buttons
	if (SPECIAL == 1){
		DB->SetXY(sidePadding, wH - sidePadding);
	}
	Exit->x = wW - sidePadding - (sidePadding/2);
	Exit->y = wH - sidePadding;
	
	Options->x = Exit->x - Options->ButtonBack.w - 20;
	Options->y = wH - sidePadding;

	Wireless->bx = Options->x - Wireless->ButtonBack.w - 20;
	Wireless->by = wH - sidePadding;
	Wireless->width = wW;
	Wireless->height = wH / 3;
	Wireless->x = 0;
	Wireless->y = 2 * wH / 3;


	//DO THE LOOP ----------------------------------
	quit = false;
	Uint32 timeStamp = 0;
	focus = true;
	SDL_Rect focusInd;
	focusInd.x = 0;
	focusInd.y = 0;
	focusInd.w = 20;
	focusInd.h = 20;

	SDL_RaiseWindow(window); //Make doubley sure we have input focus

	while (!quit){
		//EVENT ------------------------------------
		while (SDL_PollEvent(&e)){
			//If user closes the window
			if (e.type == SDL_QUIT){
				quit = true;
			}
			//Keyboard Input
			if (e.type == SDL_KEYDOWN){
				if(e.key.keysym.sym == SDLK_RIGHT) {
					goRight();
				}
				if(e.key.keysym.sym == SDLK_LEFT) {
					goLeft();
				}
				if(e.key.keysym.sym == SDLK_UP) {
					goUp();
				}
				if(e.key.keysym.sym == SDLK_DOWN) {
					goDown();
				}
				if(e.key.keysym.sym == SDLK_RETURN){
					goSelect();
				}
				if(e.key.keysym.sym == SDLK_BACKSPACE) {
					goDeselect();
				}
				if(e.key.keysym.sym == SDLK_ESCAPE)
					quit = true;
			}
			//Controller Buttons
			if(e.type == SDL_CONTROLLERBUTTONDOWN){
				if(e.cbutton.button == SDL_CONTROLLER_BUTTON_A){
					goSelect();
				}
				if(e.cbutton.button == SDL_CONTROLLER_BUTTON_B){
					goDeselect();
				}
				if(e.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_UP){
					goUp();
				}
				if(e.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_DOWN){
					goDown();
				}
				if(e.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_LEFT){
					goLeft();
				}
				if(e.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_RIGHT){
					goRight();
				}
			}
			//Aw shit, new controller
			if(e.type == SDL_CONTROLLERDEVICEADDED){
				if(!SDL_GameControllerGetAttached(controller)){ //Only run if we dont already have a controller
					scanForController();
				}
			}
			//Aw shit, removed controller
			if(e.type == SDL_CONTROLLERDEVICEREMOVED){
				if(!SDL_GameControllerGetAttached(controller)){ //If removed controller is actual controller
					logSDLMessage(std::cout, "Controller Removed");
					SDL_GameControllerClose(controller);
					//Scan for new controller
					scanForController();
				}
			}
			//Window Management
			if(e.type == SDL_WINDOWEVENT){
				if(e.window.event == SDL_WINDOWEVENT_MINIMIZED) 
					SDL_ShowWindow(window); //No hidden window for you
				if(e.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
					focus = false;
					SDL_ShowCursor(1);
				}
				if(e.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
					focus = true;
					SDL_ShowCursor(0);
				}
				if(e.window.event == SDL_WINDOWEVENT_RESIZED) {} //Tempted to add button placement code again, but it would probably fuck with animations
			}
		}
		//Controller Joysticks need to be polled because REASONS
		if( abs(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX)) > JOY_BUFFER || abs(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY)) > JOY_BUFFER){
			if(SDL_GetTicks() - timeStamp < JOY_TIME) {} //If we're still waiting on timer
			else {
				timeStamp = SDL_GetTicks();
				//RIGHT
				if(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX) > JOY_BUFFER) {
					goRight();
				}
				//LEFT
				if(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX) < -JOY_BUFFER) {
					goLeft();
				}
				timeStamp = SDL_GetTicks();
				//DOWN
				if(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY) > JOY_BUFFER) {
					goDown();
				}
				//UP
				if(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY) < -JOY_BUFFER) {
					goUp();
				}
			}
		}
		if( abs(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY)) < JOY_BUFFER && abs(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX)) < JOY_BUFFER){
			timeStamp = 0; //Reset timer if we've returned to center
		}

		//Rendering --------------------------------
		SDL_RenderClear(renderer);
		for(std::vector<Button*>::iterator ButtonListIt = ButtonList.begin(); ButtonListIt != ButtonList.end(); ButtonListIt++) {
			(*ButtonListIt)->Render(renderer);
		}
		//Wifi Things
		Wireless->Render(renderer);

		//focus indicator
		if(focus == false) {
			SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
			SDL_RenderFillRect(renderer, &focusInd);
		}

		//Set Background Color
		SDL_SetRenderDrawColor(renderer, 0x34, 0x38, 0x38, 0xFF);
		//Update the screen
		SDL_RenderPresent(renderer);
	}

	//CLEANUP --------------------------------------
	//Launch Explorer so the user isnt dumped without escape
	if (DEBUG == 0)
		iRes = (DWORD)ShellExecute(NULL, "open", "C:\\Windows\\explorer.exe","", NULL, SW_NORMAL);
	//Error checking is for suckers (not like we can do anything about it)

	//CLEANUP
	//for(std::vector<Button*>::iterator ButtonListIt = ButtonList.begin(); ButtonListIt != ButtonList.end(); ButtonListIt++) {
	//	(*ButtonListIt)->Cleanup();
	//	SDL_free((*ButtonListIt));
	//}
	Steam->Cleanup();
	Plex->Cleanup();
	Exit->Cleanup();
	Options->Cleanup();
	Wireless->Cleanup();
	//SDL_free(Steam);
	//SDL_free(Plex);
	//SDL_free(Options);
	//SDL_free(Exit);
	cleanup(font,window,renderer);
	SDL_GameControllerClose(controller);
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();

	return 0;
}


void App::goLeft(){
	if(focus == false) return;
	if(Plex->state == BUTTON_STATE_SELECTED){
		Plex->state = BUTTON_STATE_UNSELECTED;
		Steam->state = BUTTON_STATE_SELECTED;
	}
	else if(Steam->state == BUTTON_STATE_SELECTED) {}
	else if (Wireless->state == WIFI_BUTTON_STATE_SELECTED) {
		Wireless->state = WIFI_BUTTON_STATE_UNSELECTED;
		if (SPECIAL == 1){
			DB->state = BUTTON_STATE_SELECTED;
		}
		else{
			Steam->state = BUTTON_STATE_SELECTED;
		}
	}
	else if (Wireless->state == WIFI_BUTTON_STATE_ACTIVE){
		Wireless->MoveLeft();
	} 
	else if(Options->state == BUTTON_STATE_SELECTED) {
		Options->state = BUTTON_STATE_UNSELECTED;
		Wireless->state = WIFI_BUTTON_STATE_SELECTED;
	}
	else if(Options->state == BUTTON_STATE_ACTIVE) {}
	else if(Exit->state == BUTTON_STATE_SELECTED) {
		Exit->state = BUTTON_STATE_UNSELECTED;
		Options->state = BUTTON_STATE_SELECTED;
	}
	else if(Exit->state == BUTTON_STATE_ACTIVE) {}
	else if (SPECIAL == 1){
		if (DB->state == BUTTON_STATE_SELECTED){}
	}
	else
		Steam->state = BUTTON_STATE_SELECTED;
}
void App::goUp(){
	if(focus == false) return;
	if(Steam->state == BUTTON_STATE_SELECTED) {}
	else if(Plex->state == BUTTON_STATE_SELECTED) {}
	else if (Wireless->state == WIFI_BUTTON_STATE_SELECTED) {
		Wireless->state = WIFI_BUTTON_STATE_UNSELECTED;
		Plex->state = BUTTON_STATE_SELECTED;
	}
	else if (Wireless->state == WIFI_BUTTON_STATE_ACTIVE){} //TODO
	else if(Options->state == BUTTON_STATE_SELECTED) {
		Options->state = BUTTON_STATE_UNSELECTED;
		Plex->state = BUTTON_STATE_SELECTED;
	}
	else if(Options->state == BUTTON_STATE_ACTIVE) {
		Options->popMenu->MoveUp();
	}
	else if(Exit->state == BUTTON_STATE_SELECTED) {
		Exit->state = BUTTON_STATE_UNSELECTED;
		Plex->state = BUTTON_STATE_SELECTED;
	}
	else if(Exit->state == BUTTON_STATE_ACTIVE) {
		Exit->popMenu->MoveUp();
	}
	else if (SPECIAL == 1){
		if (DB->state == BUTTON_STATE_SELECTED){
			DB->state = BUTTON_STATE_UNSELECTED;
			Steam->state = BUTTON_STATE_SELECTED;
		}
	}
	else
		Steam->state = BUTTON_STATE_SELECTED;
}
void App::goRight(){
	if(focus == false) return;
	if(Steam->state == BUTTON_STATE_SELECTED){
		Steam->state = BUTTON_STATE_UNSELECTED;
		Plex->state = BUTTON_STATE_SELECTED;
	}
	else if(Plex->state == BUTTON_STATE_SELECTED) {}
	else if (Wireless->state == WIFI_BUTTON_STATE_SELECTED) {
		Wireless->state = WIFI_BUTTON_STATE_UNSELECTED;
		Options->state = BUTTON_STATE_SELECTED;
	}
	else if (Wireless->state == WIFI_BUTTON_STATE_ACTIVE){
		Wireless->MoveRight();
	}
	else if(Options->state == BUTTON_STATE_SELECTED) {
		Options->state = BUTTON_STATE_UNSELECTED;
		Exit->state = BUTTON_STATE_SELECTED;
	}
	else if(Options->state == BUTTON_STATE_ACTIVE) {}
	else if(Exit->state == BUTTON_STATE_SELECTED) {}
	else if(Exit->state == BUTTON_STATE_ACTIVE) {}
	else if (SPECIAL == 1){
		if (DB->state == BUTTON_STATE_SELECTED){
			DB->state = BUTTON_STATE_UNSELECTED;
			Wireless->state = BUTTON_STATE_SELECTED;
		}
	}
	else
		Plex->state = BUTTON_STATE_SELECTED;
}
void App::goDown(){
	if (focus == false) return;
	if (Steam->state == BUTTON_STATE_SELECTED){
		Steam->state = BUTTON_STATE_UNSELECTED;
		if (SPECIAL == 1){
			DB->state = WIFI_BUTTON_STATE_SELECTED;
		}
		else{
			Wireless->state = WIFI_BUTTON_STATE_SELECTED;
		}
	}
	else if (Plex->state == BUTTON_STATE_SELECTED){
		Plex->state = BUTTON_STATE_UNSELECTED;
		Wireless->state = WIFI_BUTTON_STATE_SELECTED;
	}
	else if (Wireless->state == WIFI_BUTTON_STATE_SELECTED) {}
	else if (Wireless->state == WIFI_BUTTON_STATE_ACTIVE){} //TODO
	else if (Options->state == BUTTON_STATE_SELECTED) {}
	else if (Options->state == BUTTON_STATE_ACTIVE) {
		Options->popMenu->MoveDown();
	}
	else if (Exit->state == BUTTON_STATE_SELECTED) {}
	else if (Exit->state == BUTTON_STATE_ACTIVE) {
		Exit->popMenu->MoveDown();
	}
	else if (SPECIAL == 1){
		if (DB->state == BUTTON_STATE_SELECTED){}
	}
	else
		Wireless->state = WIFI_BUTTON_STATE_SELECTED;
}

void App::goSelect(){
	if(focus == false) return;
	if(Exit->state == BUTTON_STATE_ACTIVE){
		if(!Exit->ActivateMenuItem())
			logSDLError(std::cout,"MenuItemLaunch");
	}
	else if(Options->state == BUTTON_STATE_ACTIVE){
		if(!Options->ActivateMenuItem())
			logSDLError(std::cout,"MenuItemLaunch");
	}
	else if (Wireless->state == WIFI_BUTTON_STATE_SELECTED || Wireless->state == WIFI_BUTTON_STATE_ACTIVE){
		if (!Wireless->Activate())
			logSDLError(std::cout, "WirelessError");
	}
	else{
		for(unsigned int i = 0;i < ButtonList.size();i++) {
			if(ButtonList[i]->state == BUTTON_STATE_SELECTED)
				if(!ButtonList[i]->Activate())
					logSDLError(std::cout,"AppLaunch");
		}
	}
}

void App::goDeselect(){
	if(focus == false) return;
	if(Exit->state == BUTTON_STATE_ACTIVE)
		Exit->Activate();
	if(Options->state == BUTTON_STATE_ACTIVE)
		Options->Activate();
	if (Wireless->state == WIFI_BUTTON_STATE_ACTIVE) {
		Wireless->DeActivate();
	}
}

void App::scanForController(){
	for (int i = 0; i < SDL_NumJoysticks(); ++i) {
		if (SDL_IsGameController(i)) {
			controller = SDL_GameControllerOpen(i);
			logSDLMessage(std::cout, "Controller Detected");
			if (controller) {
		        break;
		    } 
			else {
		        logSDLError(std::cout, "Could not open Controller");
			}
		}
	}
}
