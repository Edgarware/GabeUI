#ifndef _CONFIG_H__
#define _CONFIG_H__

#include "types.h"

void ButtonList_Cleanup();
void Config_ReadConfig(const char* filename, SDL_Renderer* renderer);
void Config_OrganizeButtons(SDL_Renderer* renderer);

#endif