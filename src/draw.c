#include "draw.h"
#include "fontcache/SDL_FontCache.h"
#include <stdio.h>

FC_Font* diag_font;
SDL_Rect window_size;
SDL_Color bg_color;

//Some helpful macros
#define RIGHT_ALLIGN(width) (window_size.w - width)
#define BOTTOM_ALLIGN(height) (window_size.h - height)

#define BUTTON_INFO_PRINT(button) "%s\n%d %d %d %d", button.name, button.pos.x, button.pos.y, button.pos.w, button.pos.h


SDL_Rect Draw_GetSize(SDL_Texture* texture) {
    SDL_Rect ret;
    ret.x = ret.y = 0;
    if(SDL_QueryTexture(texture, NULL, NULL, &ret.w, &ret.h) != 0){
        SDL_Log(SDL_GetError());
    }
    return ret;
}

void Draw_Init(SDL_Renderer *renderer){
    //Get Initial Window size
    SDL_GetRendererOutputSize(renderer, &window_size.w, &window_size.h);

    //Initialize Font
    diag_font = FC_CreateFont();
    FC_LoadFont(diag_font, renderer, "Assets/calibri.ttf", 16, FC_MakeColor(0xFF, 0xFF, 0xFF, 0xFF), TTF_STYLE_NORMAL);

    //Set Background color
    bg_color.r = 0x60;
    bg_color.g = 0x60;
    bg_color.b = 0x60;
    bg_color.a = 0xFF;
}

void Draw_Cleanup(){
    FC_FreeFont(diag_font);
}

int Draw_Screen(SDL_Renderer *renderer){
    int i;
    SDL_bool has_dev_outline = SDL_FALSE;
    union TopButton temp_button;

    //If the window has resized, recalculate size
    if(did_resize == SDL_TRUE)
        SDL_GetRendererOutputSize(renderer, &window_size.w, &window_size.h);

    SDL_SetRenderDrawColor(renderer, bg_color.r , bg_color.g, bg_color.b, bg_color.a);
    SDL_RenderClear(renderer);
    for(i = 0; i < button_num; i++){
        // Escape on null button
        if(button_list[i].type == 0)
            break;

        //Draw Button
        SDL_RenderCopy(renderer, button_list[i].button.texture, NULL, &button_list[i].button.pos);

        //If dev mode, check to see if mouse bounds are within button bounds
        if(dev_mode == SDL_TRUE && has_dev_outline == SDL_FALSE){
            if(SDL_PointInRect(&mouse_pos, &button_list[i].button.pos) == SDL_TRUE){
                //Outline Element
                SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
                SDL_RenderDrawRect(renderer, &button_list[i].button.pos);

                //Save which button it is so we can draw it last
                temp_button = button_list[i];

                //Make sure we dont try and do this for more than 1 button, will break a lot
                has_dev_outline = SDL_TRUE;
            }
        }
    }

    //If dev mode, additional stuff
    if(dev_mode == SDL_TRUE){
        //Draw FPS
        FC_Draw(diag_font, renderer, RIGHT_ALLIGN(FC_GetWidth(diag_font, "%.2f", fps)), 0, "%.2f", fps);
        //Draw Button info
        if(has_dev_outline == SDL_TRUE){
            //TODO: Could use some cleanup 
            //      May want this to be extensible to multiple button types
            FC_Draw(diag_font, renderer, RIGHT_ALLIGN(FC_GetWidth(diag_font, BUTTON_INFO_PRINT(temp_button.button))), BOTTOM_ALLIGN(FC_GetHeight(diag_font, BUTTON_INFO_PRINT(temp_button.button))), BUTTON_INFO_PRINT(temp_button.button));
        }
    }

    //Push the backbuffer
    SDL_RenderPresent(renderer);
    return 0;
}