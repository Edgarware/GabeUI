#include "event.h"

#define JOY_DEADZONE 5000

SDL_GameController* controller;
SDL_Window* window;

void Event_ScanForController(){
	for (int i = 0; i < SDL_NumJoysticks(); ++i) {
		if (SDL_IsGameController(i)) {
			controller = SDL_GameControllerOpen(i);
			if (controller)
		        break;
			else
		        SDL_Log("Controller failure: %s", SDL_GetError());
		}
	}
}

int Activate(){
    int i, j;
    for(i = 0; i < button_num; i++){
        if(button_list[i].button.state == BUTTON_STATE_SELECTED){
            button_list[i].button.state = BUTTON_STATE_ACTIVE;

            //TODO: OS_Launch return val check
            if(button_list[i].type == BUTTON_TYPE_APPBUTTON){
                OS_Launch(button_list[i].appbutton.application, button_list[i].appbutton.arguments);
                button_list[i].button.state = BUTTON_STATE_SELECTED;
            }
        
        } else if(button_list[i].button.state == BUTTON_STATE_ACTIVE && button_list[i].type == BUTTON_TYPE_MENUBUTTON){
            //Check menuitems
            for(j = 0; j < button_list[i].menubutton.menu_num; j++){
                if(button_list[i].menubutton.menu[j].state == BUTTON_STATE_SELECTED){
                    switch(button_list[i].menubutton.menu[j].type){
                        case MENUITEM_QUIT:
                            return -3;
                            break;
                        case MENUITEM_APPLICATION:
                            break;
                        case MENUITEM_SHUTDOWN:
                            break;
                        case MENUITEM_RESTART:
                            break;
                    }
                }
            }
        }
    }
    return 0;
}

void Deactivate(){
    int i;
    for(i = 0; i < button_num; i++){
        if(button_list[i].button.state == BUTTON_STATE_ACTIVE){
            button_list[i].button.state = BUTTON_STATE_SELECTED;
        }
    }
}

void Move(int direction){
    int i, j;
    for(i = 0; i < button_num; i++){
        if(button_list[i].button.state == BUTTON_STATE_SELECTED){
            if(button_list[i].button.directions[direction] != NULL){
                button_list[i].button.state = BUTTON_STATE_UNSELECTED;
                button_list[i].button.directions[direction]->button.state = BUTTON_STATE_SELECTED;
            }
            break;
        }
        if(button_list[i].button.state == BUTTON_STATE_ACTIVE && button_list[i].type == BUTTON_TYPE_MENUBUTTON){
            for(j = 0; j < button_list[i].menubutton.menu_num; j++){
                if(button_list[i].menubutton.menu[j].state == BUTTON_STATE_SELECTED){
                    switch(direction){
                        case BUTTON_DIR_UP:
                            if(j + 1 < button_list[i].menubutton.menu_num){
                                button_list[i].menubutton.menu[j].state = BUTTON_STATE_UNSELECTED;
                                button_list[i].menubutton.menu[j+1].state = BUTTON_STATE_SELECTED;
                            }
                            break;
                        case BUTTON_DIR_DOWN:
                            if(j > 0){
                                button_list[i].menubutton.menu[j].state = BUTTON_STATE_UNSELECTED;
                                button_list[i].menubutton.menu[j-1].state = BUTTON_STATE_SELECTED;
                            }
                            break;
                    }
                }
            }
        }
    }
}

void ToggleDevMode(){
    if(dev_mode == 0){
        dev_mode = SDL_TRUE;
        SDL_ShowCursor(1);
        //Make window non-windowed
        SDL_SetWindowFullscreen(window, 0);
        SDL_SetWindowBordered(window, SDL_TRUE);
    } else {
        dev_mode = SDL_FALSE;
        SDL_ShowCursor(0);
        //Make window fullscreen
        if(WINDOW_FLAGS & SDL_WINDOW_BORDERLESS){
            SDL_SetWindowBordered(window, SDL_FALSE);
        }
        if(WINDOW_FLAGS & SDL_WINDOW_FULLSCREEN_DESKTOP){
            SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
        } else if(WINDOW_FLAGS & SDL_WINDOW_FULLSCREEN){
            SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
        }
    }
}

void Event_Init(SDL_Window *in_window){
    dev_mode = SDL_FALSE;
    SDL_ShowCursor(0);
    window = in_window;

    //Check for Controllers
    SDL_GameControllerAddMappingsFromFile("gamecontrollerdb.txt");
    Event_ScanForController();
}

int Event_Handle(){
    SDL_Event event;
    int16_t joy_x, joy_y;
    
    did_resize = SDL_FALSE;

    // Handle Mouse position (dev mode)
    if(dev_mode == SDL_TRUE){
        uint32_t mouse_state = SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);
        if(mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT)){
            //TODO: Something maybe
        }
    }

    // Handle Keyboard

    // Handle Controller Joystick
    if(SDL_GameControllerGetAttached(controller)){
        joy_x = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
        joy_y = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);

        //TODO: Use a counter to make this not happen every frame
        if(abs(joy_x) > JOY_DEADZONE){
            if(joy_x < 0){
                Move(BUTTON_DIR_LEFT);
            } else {
                Move(BUTTON_DIR_RIGHT);
            }
        }
        if(abs(joy_y) > JOY_DEADZONE){
            if(joy_y < 0){
                Move(BUTTON_DIR_UP);
            } else {
                Move(BUTTON_DIR_DOWN);
            }
        }
    }

    // Handle SDL Events
    while(SDL_PollEvent(&event)){
        switch(event.type){
            case SDL_QUIT: //Close button, stop the application
                return -1;
                break;

            //Window Events
            case SDL_WINDOWEVENT:
                if(event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED){
                    //Inform the renderer
                    did_resize = SDL_TRUE;
                }
                break;

            //Controller device events
            case SDL_CONTROLLERDEVICEADDED:
                if(!SDL_GameControllerGetAttached(controller))
                    Event_ScanForController();
                break;

            case SDL_CONTROLLERDEVICEREMOVED:
                if(!SDL_GameControllerGetAttached(controller)){
                    SDL_GameControllerClose(controller);
                    Event_ScanForController();
                }
                break;

            //Controller Buttons
            case SDL_CONTROLLERBUTTONDOWN:
                if(event.cbutton.button == SDL_CONTROLLER_BUTTON_A){
                    if(Activate() == -3)
                        return -1;
                } else if(event.cbutton.button == SDL_CONTROLLER_BUTTON_B){
                    Deactivate();
                }
                break;

            //Special Case Keyboard events
            case SDL_KEYDOWN:
                if(event.key.repeat == 0){
                    if(event.key.keysym.sym == SDLK_ESCAPE){
                        return -1;
                    }
                    if(event.key.keysym.sym == SDLK_F1){
                        ToggleDevMode();
                    }
                    if(event.key.keysym.sym == SDLK_RETURN){
                        if(Activate() == -3)
                            return -1;
                    }
                    if(event.key.keysym.sym == SDLK_BACKSPACE){
                        Deactivate();
                    }
                    if(event.key.keysym.sym == SDLK_UP){
                        Move(BUTTON_DIR_UP);
                    }
                    if(event.key.keysym.sym == SDLK_DOWN){
                        Move(BUTTON_DIR_DOWN);
                    }
                    if(event.key.keysym.sym == SDLK_LEFT){
                        Move(BUTTON_DIR_LEFT);
                    }
                    if(event.key.keysym.sym == SDLK_RIGHT){
                        Move(BUTTON_DIR_RIGHT);
                    }
                }
                break;
        }
    }
    return 0;
}