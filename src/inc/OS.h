#ifndef _OS_H__
#define _OS_H__

//Use different SDL include paths depending on OS
#ifdef _WIN32
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#elif __linux__
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#elif __APPLE__
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <SDL2_ttf/SDL_ttf.h>
#endif

void OS_Init();
int OS_Launch(const char* app, const char* args);
void OS_Shutdown();
void OS_Restart();

#endif