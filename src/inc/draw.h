#ifndef _DRAW_H__
#define _DRAW_H__
#include "types.h"

void Draw_Init(SDL_Renderer *renderer);
void Draw_Cleanup();

int Draw_Screen(SDL_Renderer *renderer);
#endif