#ifndef _OS_H__
#define _OS_H__

//WINDOWS SPECIFIC CODE/DEFINES
#ifdef _WIN32
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <objbase.h>

//LINUX SPECIFIC CODE/DEFINES
#elif __linux__
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <unistd.h>
#include <stdlib.h>

#endif
#endif

void OS_Init();
int OS_Launch(char* app, char* args);
void OS_Shutdown();
void OS_Restart();