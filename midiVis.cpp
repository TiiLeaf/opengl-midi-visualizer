//
// INCLUDES
//

//graphics and windowing libraries
#include <GL/glew.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_opengl.h"

//helpers
#include "./helpers/sdlHelpers.cpp"
#include "./helpers/openGlHelpers.cpp"
#include "./helpers/globals.h"

//classes
#include "./classes/Model.hpp"
#include "./classes/ModelFactory.hpp"

//c++ libraries
#include <vector>
#include <iostream>
#include <string>

//
// GLOBALS
//

//memory space for ALL project globals should created here and used without declarations elsewhere
SDL_Window* gWindow = nullptr;
bool gShouldExit = false;
SDL_GLContext gCtx = nullptr;
const unsigned short gNumTextures = 3;
unsigned int gTextures[gNumTextures];

//all file globals go here and should never be used elsewhere
Model* pianoShell;

//
// UPDATE AND DRAW SCENE
//

/*
	Create textures and models for objects in the scene, assign them references.
*/
void buildScene() {
	//build the shell of the piano using an obj file
	gTextures[0] = loadBmpFile("./res/img/pianoTex.bmp");
	pianoShell = ModelFactory::fromObj("./res/obj/pianoShell.obj");
	pianoShell->setTextureHandle(0);
}


/*
	Update the scene before it is drawn.
*/
void update(double deltaTime) {
}

/*
	Draw the scene.
*/
void draw() {
	//clear screen
    glClearColor(0.f, 0.f, 0.1f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	//reset transformations
	glLoadIdentity();
	glTranslatef(0, 0, -10);

    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	pianoShell->draw();

    glDisable(GL_TEXTURE_2D);
}

//
//	ENTRYPOINT
//
int main(int argc, char* argv[]) {
	//initalize sdl2, exit program if initalization fails
	initSDL();

	//try to create a window, gShouldExit is false if creation fails
	createWindow("MidiVis [Sam Jansen, CSCI 4229]", 800, 600);
	setProjection(800.0f/600.0f);

	//load the resouces neccecary to draw the scene
	buildScene();

	//setup deltaTime calculations
	const double timerFrequency = SDL_GetPerformanceFrequency();
	Uint64 timerNow = SDL_GetPerformanceCounter();
	Uint64 timerLast = 0;
	double deltaTime = 0;

	//start program loop
	while (!gShouldExit) {
		//calculate deltaTime
		timerLast = timerNow;
		timerNow = SDL_GetPerformanceCounter();
		deltaTime = ((timerNow - timerLast)*1000 / timerFrequency);

		//process keyboard and window events
		processEvents();

		update(deltaTime);
		draw();

		SDL_GL_SwapWindow(gWindow);
	}

	//cleanup window and SLD before exiting
	cleanup();
    return 0;
}