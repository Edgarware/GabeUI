#include "App.h"

#define JOY_BUFFER 10000
#define JOY_TIME 200

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

void split(const std::string& str, const std::string& delim, std::vector<std::string>& parts) {
    size_t start, end = 0;
    while (end < str.size()) {
        start = end;
        while (start < str.size() && (delim.find(str[start]) != std::string::npos)) {
            start++;  // skip initial whitespace
        }
        end = start;
        while (end < str.size() && (delim.find(str[end]) == std::string::npos)) {
            end++; // skip to end of word
        }
        if (end-start != 0) {  // just ignore zero-length strings.
            parts.push_back(std::string(str, start, end-start));
        }
    }
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


App::App(){}

//Main program -----------------------------------------------------------------------------------------------------------------
int App::Main(int argc, char** argv){

	//Init SDL and Components
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
		logSDLError(std::cout, "SDL_Init");
		return 1;
	}
	if (TTF_Init() != 0){
		logSDLError(std::cout, "TTF_Init");
		return 1;
	}

	//Get window size & make window
	SDL_Rect displaySize;
    if (SDL_GetDisplayBounds(0, &displaySize) != 0) {
		logSDLError(std::cout, "CreateWindow");
		TTF_Quit();
		SDL_Quit();
		return 2;
	}

    window = SDL_CreateWindow("GabeUI", displaySize.x, displaySize.y, displaySize.w, displaySize.h, SDL_WINDOW_FULLSCREEN_DESKTOP); //we dont support resizable anymore OH WELL
    if (window == NULL){
		logSDLError(std::cout, "CreateWindow");
		TTF_Quit();
		SDL_Quit();
		return 2;
	}

	//Make a renderer and set it's properties
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL){
		logSDLError(std::cout, "CreateRenderer");
		cleanup(window);
		TTF_Quit();
		SDL_Quit();
		return 3;
	}
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND); //Handle Transparency
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best"); //How we Scale
	SDL_ShowCursor(0);

	//Load Font
	textColor = {0xFF, 0xFF, 0xFF, 0xFF};
	font = TTF_OpenFont((SDL_GetBasePath() + (std::string)"Assets/calibri.ttf").c_str(), 42);
	if (font == NULL){
		logSDLError(std::cout, "TTF_OpenFont");
		cleanup(window);
		SDL_Quit();
		return 4;
	}
	TTF_SetFontHinting(font, TTF_HINTING_LIGHT); //Makes it prettier? SDL suggested it

	//Load Controller if applicable
	controller = NULL;
	if(SDL_GameControllerAddMappingsFromFile("gamecontrollerdb.txt") == -1){
	    logSDLMessage(std::cout, "gamecontrollerdb.txt not found");
    }
	scanForController();

	quit = false;

	//Init Buttons
	std::ifstream inputfile;
	inputfile.open((SDL_GetBasePath() + (std::string)"buttons.cfg").c_str(), std::ifstream::in);
	if(!configParse(&inputfile)){
	    return 5;
	}
	inputfile.close();

    if(BottomButtonList.size() <= 0 || MainButtonList.size() <= 1){ //I need at least 2 main buttons and one menu button
        return 6;
    }

	//POSITION BUTTONS
	int wW, wH;
	int sidePadding;
	//Calculate the Size and Position of all buttons
	SDL_GetWindowSize(window, &wW, &wH);
	//side padding is 10% from the edges
	sidePadding = (int)(wH * 0.10f);

    //menu buttons
    //Do these first, so we know not to overlap them with MainButtons
	int menuwidth = 0;
	for(std::vector<MenuButton*>::reverse_iterator BottomButtonListIt = BottomButtonList.rbegin(); BottomButtonListIt != BottomButtonList.rend(); ++BottomButtonListIt){
        if(BottomButtonListIt == BottomButtonList.rbegin()) //first element
            menuwidth  = wW - sidePadding - (sidePadding/2);
        else
            menuwidth  = menuwidth - (*BottomButtonListIt)->getW() - 20;
        (*BottomButtonListIt)->setX(menuwidth);
        (*BottomButtonListIt)->setY(wH - sidePadding);
	}

	//main buttons
	int numrow = ((MainButtonList.size() - 1) /2.0f);

	SDL_Rect maincanvas;
	maincanvas.x = sidePadding;
	maincanvas.y = sidePadding;
	maincanvas.w = wW - sidePadding*2.0f;
	maincanvas.h = wH - sidePadding*2.0f;
	int elementHeight = (maincanvas.h/(numrow+1.0f)) - sidePadding*(1.0f/2.0f)*((int)(numrow/2));
	int elementWidth = (maincanvas.w/2.0f) - (sidePadding/4.0f);
	int mainwidth = 0;
	int mainheight = maincanvas.y;
	int count = 0;
	for(std::vector<MainButton*>::iterator MainButtonListIt = MainButtonList.begin(); MainButtonListIt != MainButtonList.end(); MainButtonListIt++){
        if(MainButtonListIt == MainButtonList.begin()){ //FIRST ELEMENT
            (*MainButtonListIt)->SetProportionalSizeH(elementHeight); //set width to check if overflow
            if((*MainButtonListIt)->getW() > elementWidth){ //TOO BIG, use height to determine width instead
                (*MainButtonListIt)->SetProportionalSizeW(elementWidth);
                elementHeight = (*MainButtonListIt)->getH();
            }
            else{
                elementWidth = (*MainButtonListIt)->getW();
            }
            //Now that we know the size of each element, change canvas size and center
            maincanvas.w = 2.0f * elementWidth + (sidePadding/2.0f);
            maincanvas.x = (wW/2.0f)-(maincanvas.w/2.0f);

        }
        else{
            (*MainButtonListIt)->SetProportionalSizeW(elementWidth); //I trust the width set function more
        }
        (*MainButtonListIt)->SetXY(maincanvas.x+mainwidth,mainheight);


        if(count%2 == 0)
            mainwidth = elementWidth + (sidePadding/2.0f);
        else
            mainwidth = 0;
        count++;

        if(count%2 == 0){ //if second element
            mainheight += elementHeight + (sidePadding/2.0f);
        }
	}


	//DO THE LOOP ----------------------------------
	Uint32 timeStamp = 0;
	focus = true;
	SDL_Rect focusInd;
	focusInd.x = 0;
	focusInd.y = 0;
	focusInd.w = 20;
	focusInd.h = 20;
	selected = -1;

	SDL_RaiseWindow(window); //Make doubley sure we have input focus

	while (!quit){
		//EVENT ------------------------------------
		while (SDL_PollEvent(&e)){
			//If user closes the window (close button)
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
			//new controller
			if(e.type == SDL_CONTROLLERDEVICEADDED){
				if(!SDL_GameControllerGetAttached(controller)){ //Only run if we dont already have a controller
					scanForController();
				}
			}
			//removed controller
			if(e.type == SDL_CONTROLLERDEVICEREMOVED){
				if(!SDL_GameControllerGetAttached(controller)){ //If removed controller is actual removed
					logSDLMessage(std::cout, "Controller Removed");
					SDL_GameControllerClose(controller);
					//Scan for new controller
					scanForController();
				}
			}
			//Window Management
			if(e.type == SDL_WINDOWEVENT){
				if(e.window.event == SDL_WINDOWEVENT_MINIMIZED){
					SDL_RestoreWindow(window); //No hidden window for you
                }
                if(e.window.event == SDL_WINDOWEVENT_LEAVE) {
					focus = false;
					SDL_ShowCursor(1);
				}
				if(e.window.event == SDL_WINDOWEVENT_ENTER) {
					focus = true;
					SDL_ShowCursor(0);
				}
				if(e.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
					focus = false;
					SDL_ShowCursor(1);
				}
				if(e.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
					focus = true;
					SDL_ShowCursor(0);
				}
			}
		}
		//Controller Joysticks need to be polled so they arent terrible
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
			timeStamp = 0; //Reset timer if we've returned to center(ish)
		}

		//Rendering --------------------------------
		SDL_RenderClear(renderer);
		for(std::vector<MainButton*>::iterator MainButtonListIt = MainButtonList.begin(); MainButtonListIt != MainButtonList.end(); MainButtonListIt++) {
			(*MainButtonListIt)->Render(renderer);
		}
		for(std::vector<MenuButton*>::iterator MenuButtonListIt = BottomButtonList.begin(); MenuButtonListIt != BottomButtonList.end(); MenuButtonListIt++) {
			(*MenuButtonListIt)->Render(renderer);
		}

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

	for(std::vector<MainButton*>::iterator MainButtonListIt = MainButtonList.begin(); MainButtonListIt != MainButtonList.end(); MainButtonListIt++) {
		(*MainButtonListIt)->Cleanup();
		delete (*MainButtonListIt);
	}
	for(std::vector<MenuButton*>::iterator MenuButtonListIt = BottomButtonList.begin(); MenuButtonListIt != BottomButtonList.end(); MenuButtonListIt++) {
		(*MenuButtonListIt)->Cleanup();
		delete (*MenuButtonListIt);
	}
	cleanup(font,window,renderer);
	if(!SDL_GameControllerGetAttached(controller)){ //If removed controller is actual removed
        SDL_GameControllerClose(controller);
    }
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();

	return 0;
}
//DATA PARSING ---------------------------------------
bool App::configParse(std::ifstream *config){
    std::vector<std::string> tokens;
    std::string delim = " \t\"\'}{=";
    std::string tempLine;

    if(!config->is_open()){
        logSDLMessage(std::cout, "buttons.cfg ERROR, file unopenable");
        return false;
    }
    while(getline(*config,tempLine)){
        split(tempLine, delim, tokens);
    }

    MenuButtonElements tempMenu;
    MenuItemElements tempItem;
    MainButtonElements tempMain;
    char state = PARSE_STATE_NULL;
    for(std::vector<std::string>::iterator tokensIt = tokens.begin(); tokensIt != tokens.end(); tokensIt++) {
        std::string lowerTemp = (*tokensIt);
        std::transform(lowerTemp.begin(), lowerTemp.end(), lowerTemp.begin(), ::tolower);
		switch(state){
		    case PARSE_STATE_NULL:
                if(lowerTemp == "mainbutton")
                    state = PARSE_STATE_MAIN;
                else if(lowerTemp == "menubutton")
                    state = PARSE_STATE_MENU;
                else{ //Things broke
                    logSDLMessage(std::cout, "buttons.cfg ERROR");
                    return false;
                }
                break;
            case PARSE_STATE_MAIN:
                if(lowerTemp == "icon" || lowerTemp == "image"){
                    if(tokensIt + 1 !=tokens.end()){
                        tempMain.image = *(tokensIt + 1);
                        tokensIt++;
                    }
                    else{
                        logSDLMessage(std::cout, "buttons.cfg ERROR");
                        return false;
                    }
                }
                else if(lowerTemp == "app" || lowerTemp == "application"){
                    if(tokensIt + 1 !=tokens.end()){
                        tempMain.app = *(tokensIt + 1);
                        tokensIt++;
                    }
                    else{
                        logSDLMessage(std::cout, "buttons.cfg ERROR");
                        return false;
                    }
                }
                else if(lowerTemp == "mainbutton" || lowerTemp == "menubutton"){ //check if complete, then add to list
                    if(tempMain.app.empty() || tempMain.image.empty()){
                        logSDLMessage(std::cout, "buttons.cfg ERROR, incomplete mainbutton");
                        return false;
                    }
                    else{
                        MainButton *Temp = new MainButton;
                        if(!Temp->Init((SDL_GetBasePath() + tempMain.image),renderer)) {
                            logSDLMessage(std::cout, "buttons.cfg ERROR, mainbutton could not be loaded");
                            return false;
                        }
                        Temp->setApp(tempMain.app.c_str());
                        MainButtonList.push_back(Temp);
                        tempMain.app.erase(tempMain.app.begin(), tempMain.app.end());
                        tempMain.image.erase(tempMain.image.begin(), tempMain.image.end());
                        if(lowerTemp == "menubutton")
                            state = PARSE_STATE_MENU;
                    }
                }
                else{ //Things broke
                    logSDLMessage(std::cout, "buttons.cfg ERROR, unexpected element " + (*tokensIt));
                    return false;
                }
                break;
            case PARSE_STATE_MENU:
                if(lowerTemp == "icon" || lowerTemp == "image"){
                    if(tokensIt + 1 !=tokens.end()){
                        tempMenu.image = *(tokensIt + 1);
                        tokensIt++;
                    }
                    else{
                        logSDLMessage(std::cout, "buttons.cfg ERROR");
                        return false;
                    }
                }
                else if(lowerTemp == "menuitem"){
                    state = PARSE_STATE_ITEM;
                }
                else if(lowerTemp == "mainbutton" || lowerTemp == "menubutton"){ //check if complete, then add to list
                    if(tempMenu.image.empty() || tempMenu.items.size() <= 0){
                        logSDLMessage(std::cout, "buttons.cfg ERROR, incomplete mainbutton");
                        return false;
                    }
                    else{
                        MenuButton *Temp = new MenuButton;
                        if(!Temp->Init((SDL_GetBasePath() + tempMenu.image),renderer)) {
                            logSDLMessage(std::cout, "buttons.cfg ERROR, menubutton could not be loaded");
                            return false;
                        }
                        for(std::vector<MenuItemElements>::iterator itemsIt = tempMenu.items.begin(); itemsIt != tempMenu.items.end(); itemsIt++) { //PARSE_STATE_ITEM checks for completeness, so no need to check here
                            if((*itemsIt).type == PARSE_MENUITEM_APPLAUNCH){
                                if(!Temp->LoadItem((*itemsIt).name, font, textColor, renderer, MENUITEM_TYPE_APPLAUNCH, (*itemsIt).app.c_str(),""))
                                    return false;
                            }
                            else if((*itemsIt).type == PARSE_MENUITEM_QUIT){
                                if(!Temp->LoadItem((*itemsIt).name, font, textColor, renderer, MENUITEM_TYPE_QUIT, &quit))
                                    return false;
                            }
                            else if((*itemsIt).type == PARSE_MENUITEM_REBOOT){
                                if(!Temp->LoadItem((*itemsIt).name, font, textColor, renderer, MENUITEM_TYPE_SHUTDOWN, MENUITEM_SHUTDOWN_REBOOT))
                                    return false;
                            }
                            else if((*itemsIt).type == PARSE_MENUITEM_SHUTDOWN){
                                if(!Temp->LoadItem((*itemsIt).name, font, textColor, renderer, MENUITEM_TYPE_SHUTDOWN, MENUITEM_SHUTDOWN_SHUTDOWN))
                                    return false;
                            }
                        }
                        BottomButtonList.push_back(Temp);

                        tempMenu.image.erase(tempMenu.image.begin(), tempMenu.image.end());
                        tempMenu.items.clear();

                        if(lowerTemp == "mainbutton")
                            state = PARSE_STATE_MAIN;
                    }
                }
                else{ //Things broke
                    logSDLMessage(std::cout, "buttons.cfg ERROR, unexpected element " + (*tokensIt));
                    return false;
                }
                break;
            case PARSE_STATE_ITEM:
                if(lowerTemp == "name" || lowerTemp == "text"){
                    if(tokensIt + 1 != tokens.end()){
                        tempItem.name = *(tokensIt + 1);
                        tokensIt++;
                    }
                    else{
                        logSDLMessage(std::cout, "buttons.cfg ERROR");
                        return false;
                    }
                }
                else if(lowerTemp == "app" || lowerTemp == "application"){
                    if(tokensIt + 1 !=tokens.end()){
                        tempItem.app = *(tokensIt + 1);
                        tokensIt++;
                    }
                    else{
                        logSDLMessage(std::cout, "buttons.cfg ERROR");
                        return false;
                    }
                }
                else if(lowerTemp == "type"){
                    if(tokensIt + 1 !=tokens.end()){
                        lowerTemp = *(tokensIt + 1);
                        std::transform(lowerTemp.begin(), lowerTemp.end(), lowerTemp.begin(), ::tolower);
                        if (lowerTemp == "shutdown")
                            tempItem.type = PARSE_MENUITEM_SHUTDOWN;
                        else if (lowerTemp == "restart" || lowerTemp == "reboot")
                            tempItem.type = PARSE_MENUITEM_REBOOT;
                        else if (lowerTemp == "quit")
                            tempItem.type = PARSE_MENUITEM_QUIT;
                        else if (lowerTemp == "app" || lowerTemp == "applaunch")
                            tempItem.type = PARSE_MENUITEM_APPLAUNCH;
                        else{
                            logSDLMessage(std::cout, "buttons.cfg ERROR, unknown type " + *(tokensIt+1));
                            return false;
                        }
                        tokensIt++;
                    }
                    else{
                        logSDLMessage(std::cout, "buttons.cfg ERROR");
                        return false;
                    }
                }
                else if(lowerTemp == "menuitem"){ //Add item to menulist
                    if(tempItem.name.empty() || tempItem.type == 0 || (tempItem.type == 4 && tempItem.app.empty())){
                        logSDLMessage(std::cout, "buttons.cfg ERROR, incomplete menuitem");
                        return false;
                    }
                    else{
                        tempMenu.items.push_back(tempItem);
                        tempItem.name.erase(tempItem.name.begin(), tempItem.name.end());
                        tempItem.app.erase(tempItem.app.begin(), tempItem.app.end());
                        tempItem.type = 0;
                    }
                }
                else if(lowerTemp == "mainbutton" || lowerTemp == "menubutton"){ //check if complete, then add to list
                    if(tempMenu.image.empty() || tempMenu.items.size() <= 0){
                        logSDLMessage(std::cout, "buttons.cfg ERROR, incomplete mainbutton");
                        return false;
                    }
                    else{
                        MenuButton *Temp = new MenuButton;
                        if(!Temp->Init((SDL_GetBasePath() + tempMenu.image),renderer)) {
                            logSDLMessage(std::cout, "buttons.cfg ERROR, menubutton could not be loaded");
                            return false;
                        }
                        for(std::vector<MenuItemElements>::iterator itemsIt = tempMenu.items.begin(); itemsIt != tempMenu.items.end(); itemsIt++) { //PARSE_STATE_ITEM checks for completeness, so no need to check here
                            if((*itemsIt).type == PARSE_MENUITEM_APPLAUNCH){
                                if(!Temp->LoadItem((*itemsIt).name, font, textColor, renderer, MENUITEM_TYPE_APPLAUNCH, (*itemsIt).app.c_str(),""))
                                    return false;
                            }
                            else if((*itemsIt).type == PARSE_MENUITEM_QUIT){
                                if(!Temp->LoadItem((*itemsIt).name, font, textColor, renderer, MENUITEM_TYPE_QUIT, &quit))
                                    return false;
                            }
                            else if((*itemsIt).type == PARSE_MENUITEM_REBOOT){
                                if(!Temp->LoadItem((*itemsIt).name, font, textColor, renderer, MENUITEM_TYPE_SHUTDOWN, MENUITEM_SHUTDOWN_REBOOT))
                                    return false;
                            }
                            else if((*itemsIt).type == PARSE_MENUITEM_SHUTDOWN){
                                if(!Temp->LoadItem((*itemsIt).name, font, textColor, renderer, MENUITEM_TYPE_SHUTDOWN, MENUITEM_SHUTDOWN_SHUTDOWN))
                                    return false;
                            }
                        }
                        BottomButtonList.push_back(Temp);

                        tempMenu.image.erase(tempMenu.image.begin(), tempMenu.image.end());
                        tempMenu.items.clear();

                        if(lowerTemp == "mainbutton")
                            state = PARSE_STATE_MAIN;
                    }
                }
                else{ //Things broke
                    logSDLMessage(std::cout, "buttons.cfg ERROR, unexpected element " + (*tokensIt));
                    return false;
                }
                break;

            default:
                return false;
                break;
		}
	}
	//We broke out of the file, but we need to close the last object
    if(state == PARSE_STATE_MAIN){
        if(tempMain.app.empty() || tempMain.image.empty()){
            logSDLMessage(std::cout, "buttons.cfg ERROR, incomplete mainbutton");
            return false;
        }
        else{
            MainButton *Temp = new MainButton;
            if(!Temp->Init((SDL_GetBasePath() + tempMain.image),renderer)) {
                logSDLMessage(std::cout, "buttons.cfg ERROR, mainbutton could not be loaded");
                return false;
            }
            Temp->setApp(tempMain.app.c_str());
            MainButtonList.push_back(Temp);
        }
    }
    else if(state == PARSE_STATE_ITEM){
        if(tempItem.name.empty() || tempItem.type == 0 || (tempItem.type == 4 && tempItem.app.empty())){
            logSDLMessage(std::cout, "buttons.cfg ERROR, incomplete menuitem");
            return false;
        }
        else{
            tempMenu.items.push_back(tempItem);
        }
        if(tempMenu.image.empty() || tempMenu.items.size() <= 0){
            logSDLMessage(std::cout, "buttons.cfg ERROR, incomplete menubutton");
            return false;
        }
        else{
            MenuButton *Temp = new MenuButton;
            if(!Temp->Init((SDL_GetBasePath() + tempMenu.image),renderer)) {
                logSDLMessage(std::cout, "buttons.cfg ERROR, menubutton could not be loaded");
                return false;
            }
            for(std::vector<MenuItemElements>::iterator itemsIt = tempMenu.items.begin(); itemsIt != tempMenu.items.end(); itemsIt++) { //PARSE_STATE_ITEM checks for completeness, so no need to check here
                if((*itemsIt).type == PARSE_MENUITEM_APPLAUNCH){
                    if(!Temp->LoadItem((*itemsIt).name, font, textColor, renderer, MENUITEM_TYPE_APPLAUNCH, (*itemsIt).app.c_str(),""))
                        return false;
                }
                else if((*itemsIt).type == PARSE_MENUITEM_QUIT){
                    if(!Temp->LoadItem((*itemsIt).name, font, textColor, renderer, MENUITEM_TYPE_QUIT, &quit))
                        return false;
                }
                else if((*itemsIt).type == PARSE_MENUITEM_REBOOT){
                    if(!Temp->LoadItem((*itemsIt).name, font, textColor, renderer, MENUITEM_TYPE_SHUTDOWN, MENUITEM_SHUTDOWN_REBOOT))
                        return false;
                }
                else if((*itemsIt).type == PARSE_MENUITEM_SHUTDOWN){
                    if(!Temp->LoadItem((*itemsIt).name, font, textColor, renderer, MENUITEM_TYPE_SHUTDOWN, MENUITEM_SHUTDOWN_SHUTDOWN))
                        return false;
                }
            }
            BottomButtonList.push_back(Temp);
        }
    }
    else if(state == PARSE_STATE_MENU){
        if(tempMenu.image.empty() || tempMenu.items.size() <= 0){
            logSDLMessage(std::cout, "buttons.cfg ERROR, incomplete menubutton");
            return false;
        }
        else{
            MenuButton *Temp = new MenuButton;
            if(!Temp->Init((SDL_GetBasePath() + tempMenu.image),renderer)) {
                logSDLMessage(std::cout, "buttons.cfg ERROR, menubutton could not be loaded");
                return false;
            }
            for(std::vector<MenuItemElements>::iterator itemsIt = tempMenu.items.begin(); itemsIt != tempMenu.items.end(); itemsIt++) { //PARSE_STATE_ITEM checks for completeness, so no need to check here
                if((*itemsIt).type == PARSE_MENUITEM_APPLAUNCH){
                    if(!Temp->LoadItem((*itemsIt).name, font, textColor, renderer, MENUITEM_TYPE_APPLAUNCH, (*itemsIt).app.c_str(),""))
                        return false;
                }
                else if((*itemsIt).type == PARSE_MENUITEM_QUIT){
                    if(!Temp->LoadItem((*itemsIt).name, font, textColor, renderer, MENUITEM_TYPE_QUIT, &quit))
                        return false;
                }
                else if((*itemsIt).type == PARSE_MENUITEM_REBOOT){
                    if(!Temp->LoadItem((*itemsIt).name, font, textColor, renderer, MENUITEM_TYPE_SHUTDOWN, MENUITEM_SHUTDOWN_REBOOT))
                        return false;
                }
                else if((*itemsIt).type == PARSE_MENUITEM_SHUTDOWN){
                    if(!Temp->LoadItem((*itemsIt).name, font, textColor, renderer, MENUITEM_TYPE_SHUTDOWN, MENUITEM_SHUTDOWN_SHUTDOWN))
                        return false;
                }
                return false;
            }
            BottomButtonList.push_back(Temp);
        }
    }
    return true;
}

//MOVEMENT FUNCTIONS ---------------------------------
void App::goLeft(){
    if(focus == false){ return; }
	if(selected == -1){
        selected = 0;
        MainButtonList[selected]->setState(BUTTON_STATE_SELECTED);
	}
	else if(selected < 100){ //Selected is within main canvas
        if(selected > 0){
            if(selected%2 == 1){
                MainButtonList[selected]->setState(BUTTON_STATE_UNSELECTED);
                selected = selected - 1;
                MainButtonList[selected]->setState(BUTTON_STATE_SELECTED);
            }
        }
	}
	else{
        if(selected > 100){
            BottomButtonList[selected - 100]->setState(BUTTON_STATE_UNSELECTED);
            selected = selected - 1;
            BottomButtonList[selected - 100]->setState(BUTTON_STATE_SELECTED);
        }
	}
}
void App::goUp(){
    if(focus == false){ return; }
	if(selected == -1){
        selected = 0;
        MainButtonList[selected]->setState(BUTTON_STATE_SELECTED);
	}
	else if(selected < 100){ //Selected is within main canvas
        if(selected > 1){
            MainButtonList[selected]->setState(BUTTON_STATE_UNSELECTED);
            selected = selected - 2;
            MainButtonList[selected]->setState(BUTTON_STATE_SELECTED);
        }
	}
	else{
	    //NEED TO CHECK IF MENU IS OPEN
	    if(BottomButtonList[selected - 100]->getState() == BUTTON_STATE_ACTIVE){
            BottomButtonList[selected - 100]->MoveUp();
	    }
	    else {
	        if(MainButtonList.size()%2 == 1){
                BottomButtonList[selected - 100]->setState(BUTTON_STATE_UNSELECTED);
                selected = MainButtonList.size() - 2;
                MainButtonList[selected]->setState(BUTTON_STATE_SELECTED);
	        }
	        else{
                BottomButtonList[selected - 100]->setState(BUTTON_STATE_UNSELECTED);
                selected = MainButtonList.size() - 1;
                MainButtonList[selected]->setState(BUTTON_STATE_SELECTED);
	        }
	    }
	}
}
void App::goRight(){
    if(focus == false){ return; }
	if(selected == -1){
        selected = 1;
        MainButtonList[selected]->setState(BUTTON_STATE_SELECTED);
	}
	else if(selected < 100){ //Selected is within main canvas
        if(selected < MainButtonList.size() - 1){
            if(selected%2 == 0){
                MainButtonList[selected]->setState(BUTTON_STATE_UNSELECTED);
                selected = selected + 1;
                MainButtonList[selected]->setState(BUTTON_STATE_SELECTED);
            }
        }
	}
	else{
        if(selected < BottomButtonList.size() + 99){
            BottomButtonList[selected - 100]->setState(BUTTON_STATE_UNSELECTED);
            selected = selected + 1;
            BottomButtonList[selected - 100]->setState(BUTTON_STATE_SELECTED);
        }
	}
}
void App::goDown(){
    if(focus == false){ return; }
	if(selected == -1){
        selected = 100;
        BottomButtonList[selected - 100]->setState(BUTTON_STATE_SELECTED);
	}
	else if(selected < 100){ //Selected is within main canvas
        if(selected < MainButtonList.size() - 2 || (selected < MainButtonList.size() - 2 && MainButtonList.size()%2 == 1)){ //Check if we move to bottom bar
            if(MainButtonList.size() - 1 >= selected + 2){ //Check if we can actually move down
                MainButtonList[selected]->setState(BUTTON_STATE_UNSELECTED);
                selected = selected + 2;
                MainButtonList[selected]->setState(BUTTON_STATE_SELECTED);
            }
            else if(MainButtonList.size() - 1 >= selected + 1){ //Check if we can move down just one row
                MainButtonList[selected]->setState(BUTTON_STATE_UNSELECTED);
                selected = selected + 1;
                MainButtonList[selected]->setState(BUTTON_STATE_SELECTED);
            }
        }
        else{
            MainButtonList[selected]->setState(BUTTON_STATE_UNSELECTED);
            selected = 100;
            BottomButtonList[selected - 100]->setState(BUTTON_STATE_SELECTED);
        }
	}
	else{
	    if(BottomButtonList[selected - 100]->getState() == BUTTON_STATE_ACTIVE){
            BottomButtonList[selected - 100]->MoveDown();
	    }
        //Cant go down if in bottom bar
	}
}

void App::goSelect(){
    if(focus == false){ return; }
	for(std::vector<MainButton*>::iterator MainButtonListIt = MainButtonList.begin(); MainButtonListIt != MainButtonList.end(); MainButtonListIt++){
        if((*MainButtonListIt)->getState() == BUTTON_STATE_SELECTED){
            if(!(*MainButtonListIt)->Activate())
                logSDLMessage(std::cout,"AppLaunch");
            SDL_PumpEvents();
            SDL_FlushEvents(SDL_JOYAXISMOTION, SDL_CONTROLLERDEVICEADDED); //Controllers have a nasty habit of still sending events when unfocused, so we clear them

        }
	}
	for(std::vector<MenuButton*>::iterator BottomButtonListIt = BottomButtonList.begin(); BottomButtonListIt != BottomButtonList.end(); BottomButtonListIt++){
        if((*BottomButtonListIt)->getState() == BUTTON_STATE_SELECTED){
			if((*BottomButtonListIt)->Activate() == false)
                logSDLMessage(std::cout,"AppLaunch");
        }
        else if((*BottomButtonListIt)->getState() == BUTTON_STATE_ACTIVE){
            if(!(*BottomButtonListIt)->ActivateMenuItem())
                logSDLMessage(std::cout,"AppLaunch");
            SDL_PumpEvents();
            SDL_FlushEvents(SDL_JOYAXISMOTION, SDL_CONTROLLERDEVICEADDED); //Controllers have a nasty habit of still sending events when unfocused, so we clear them
        }
	}
}

void App::goDeselect(){
    if(focus == false){ return; }
	for(std::vector<MenuButton*>::iterator BottomButtonListIt = BottomButtonList.begin(); BottomButtonListIt != BottomButtonList.end(); BottomButtonListIt++){
        if((*BottomButtonListIt)->getState() == BUTTON_STATE_ACTIVE){
			if((*BottomButtonListIt)->Activate() == -1)
                logSDLMessage(std::cout,"MenuItemDelaunch");
        }
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
