#ifndef _OS_H__
#define _OS_H__

//WINDOWS SPECIFIC CODE/DEFINES
#ifdef _WIN32
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

//#include <windows.h>
#include <tchar.h>

//LINUX SPECIFIC CODE/DEFINES
#elif __linux__
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

#endif
#endif