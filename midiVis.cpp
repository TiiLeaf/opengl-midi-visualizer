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
#include "./classes/Object.hpp"
#include "./classes/Piano.hpp"

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
Camera gCamera(0, 7.5, 9);

//
// UPDATE AND DRAW SCENE
//

/*
	Create textures and models for objects in the scene, assign them references.
*/
std::vector<Object*> buildScene() {
	std::vector<Object*> scene;

	//load the texture and .obj model for the shell of the piano (the keys and strings are created dynamically in the `Piano` constructor)
	gTextures[gTextureHandles::PIANO_SHELL] = loadBmpFile("./res/img/pianoShell.bmp");
	gTextures[gTextureHandles::WHITE_KEY] = loadBmpFile("./res/img/whiteKey.bmp");
	gTextures[gTextureHandles::BLACK_KEY] = loadBmpFile("./res/img/blackKey.bmp");
	Model* pianoShellModel = ModelFactory::fromObj("./res/obj/pianoShell.obj");
	pianoShellModel->setTextureHandle(gTextureHandles::PIANO_SHELL);

	Piano* piano = new Piano(pianoShellModel);
	scene.push_back(piano);
	
	return scene;
}


/*
	Update the scene before it is drawn.
*/
void update(std::vector<Object*> scene, double deltaTime) {
	for (size_t i = 0; i < scene.size(); i++) {
		scene.at(i)->update(deltaTime);
	}
}

/*
	Draw the scene.
*/
void draw(std::vector<Object*> scene) {
	//clear screen
    glClearColor(0.f, 0.f, 0.1f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	//reset transformations
	glLoadIdentity();
	gCamera.setModelViewMatrix();

    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	for (size_t i = 0; i < scene.size(); i++) {
		scene.at(i)->draw();
	}

    glDisable(GL_TEXTURE_2D);
	//drawAxes();
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
	std::vector<Object*> scene = buildScene();

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
		double camDeltaTheta = 0;
		double camDeltaY = 0;
		processEvents(camDeltaTheta, camDeltaY);

		//move the camera based on the input
		if (camDeltaTheta != 0 || camDeltaY != 0) {
			gCamera.move(camDeltaTheta, camDeltaY, deltaTime);
		}

		update(scene, deltaTime); //update the scene
		draw(scene); //draw the scene

		SDL_GL_SwapWindow(gWindow);
	}

	//cleanup scene objects
	for (size_t i = 0; i < scene.size(); i++) {
		delete scene.at(i);
	}
	

	//cleanup window and SLD before exiting
	cleanup();
    return 0;
}