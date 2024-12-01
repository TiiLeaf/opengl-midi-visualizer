#ifndef GLOBALS_H
#define GLOBALS_H

#include "SDL2/SDL.h"
#include "../classes/Camera.hpp"

extern SDL_Window* gWindow;
extern SDL_GLContext gCtx;
extern bool gShouldExit;
extern const unsigned short gNumTextures;
extern unsigned int gTextures[];
extern Camera gCamera;
enum gTextureHandles {
	TEST,
	PIANO_SHELL,
	WHITE_KEY,
	BLACK_KEY,
	LAMP_POST,
	LAMP_SHADE,
	LAMP_LIGHT,
	SKYBOX_HOR,
	FLOOR,
	CEMENT
};

#endif