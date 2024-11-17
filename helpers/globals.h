#ifndef GLOBALS_H
#define GLOBALS_H

#include "SDL2/SDL.h"

extern SDL_Window* gWindow;
extern SDL_GLContext gCtx;
extern bool gShouldExit;
extern const unsigned short gNumTextures;
extern unsigned int gTextures[];

#endif