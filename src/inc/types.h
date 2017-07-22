#ifndef _TYPES_H__
#define _TYPES_H__

#include <stdint.h>
#include "OS.h"

#define WINDOW_FLAGS SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_BORDERLESS
#define BUTTON_LIST_MAX 25

/*
 * Button types are defined here
 * BaseButton is the main type, use the type_id to determine which struct to use
 */
enum button_type {
    BUTTON_TYPE_NONE,
    BUTTON_TYPE_APPBUTTON,
    BUTTON_TYPE_MENUBUTTON
};

struct BaseButton {
    uint32_t type;
    char* name;
    SDL_Texture* texture;
    SDL_Rect pos;   
};

struct AppButton {
    uint32_t type;
    char* name;
    SDL_Texture* texture;
    SDL_Rect pos; 
    char* application;
    char* arguments;
};

struct MenuButton{
    uint32_t type;
    char* name;
    SDL_Texture* texture;
    SDL_Rect pos;
};

union TopButton {
    uint32_t type;
    struct BaseButton button;
    struct AppButton appbutton;
    struct MenuButton menubutton;
};

union TopButton button_list[BUTTON_LIST_MAX];

//Common Variables
uint32_t button_num;
float fps;
SDL_bool dev_mode;
SDL_Point mouse_pos;
SDL_bool did_resize;

#endif