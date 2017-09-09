#ifndef _TYPES_H__
#define _TYPES_H__

#include <stdint.h>
#include "OS.h"

#define WINDOW_FLAGS (SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_BORDERLESS)
#define BUTTON_LIST_MAX 25
#define MENUITEM_LIST_MAX 10

/*
 * Button types are defined here
 * BaseButton is the main type, use the type_id to determine which struct to use
 */
enum button_type {
    BUTTON_TYPE_NONE,
    BUTTON_TYPE_APPBUTTON,
    BUTTON_TYPE_MENUBUTTON,
    BUTTON_TYPE_MENUITEM //Reserved for DebugDraw
};

//Direction information
enum button_dir {
    BUTTON_DIR_DOWN,
    BUTTON_DIR_UP,
    BUTTON_DIR_LEFT,
    BUTTON_DIR_RIGHT
};

//menu type
enum menuitem_type {
    MENUITEM_NONE,
    MENUITEM_QUIT,
    MENUITEM_SHUTDOWN,
    MENUITEM_RESTART,
    MENUITEM_APPLICATION
};


//State information
enum button_state {
    BUTTON_STATE_UNSELECTED,
    BUTTON_STATE_SELECTED,
    BUTTON_STATE_ACTIVE
};

struct BaseButton {
    uint32_t type;
    char* name;
    uint32_t state;
    SDL_Texture* texture;
    SDL_Rect pos; //Size of button
    SDL_Rect base_size; //Size of texture
    union TopButton* directions[4]; //Pointer to the button in a particular direction from this one
};

struct AppButton {
    uint32_t type;
    char* name;
    uint32_t state;
    SDL_Texture* texture;
    SDL_Rect pos; 
    SDL_Rect base_size;
    union TopButton* directions[4];
    char* application;
    char* arguments;
};

struct MenuItem {
    //MenuItems is NOT compatible with TopButton, though the struct is similar
    uint32_t type;
    char* name;
    uint32_t state;
    SDL_Texture* texture;
    SDL_Rect pos;
    SDL_Rect text_size;
    char* application;
    char* arguments;
};

struct MenuButton{
    uint32_t type;
    char* name;
    uint32_t state;
    SDL_Texture* texture;
    SDL_Rect pos;
    SDL_Rect base_size;
    union TopButton* directions[4];
    struct MenuItem *menu[MENUITEM_LIST_MAX];
    uint32_t menu_num;
};

union TopButton {
    uint32_t type;
    struct BaseButton button;
    struct AppButton appbutton;
    struct MenuButton menubutton;
};

union TopButton *button_list[BUTTON_LIST_MAX];
uint32_t button_num;


SDL_Window *window;
SDL_Renderer *renderer;

//Common Variables
float fps;
SDL_bool dev_mode;
SDL_Point mouse_pos;
SDL_bool did_resize;
SDL_bool did_config_modify;
SDL_bool has_focus;
TTF_Font *font;

#endif