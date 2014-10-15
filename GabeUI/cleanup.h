#ifndef CLEANUP_H
#define CLEANUP_H

#include <SDL.h>
#include <SDL_ttf.h>

/*
 * These serve to free the passed argument and also provide the
 * base cases for the template recursion
 */
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
/*
 * Clean up each argument in the list
 */
template<typename A, typename B, typename C>
void cleanup(A *a, B *b, C *c){
	cleanup(a);
	cleanup(b);
	cleanup(c);
}

#endif

