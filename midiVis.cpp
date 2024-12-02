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
#include "./classes/Lamp.hpp"
#include "./classes/Ground.hpp"
#include "./classes/Song.hpp"

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
const unsigned short gNumTextures = 12;
unsigned int gTextures[gNumTextures];
Camera gCamera(0, 7, 10);

//all file globals go here and should never be used elsewhere
Song song;
float lightPosition[4]  = {0.0f, 7.0f, 0.0f, 1.0f};
Model* skyBox;

//
// UPDATE AND DRAW SCENE
//

/*
	Create textures and models for objects in the scene, assign them references.
*/
std::vector<Object*> buildScene() {
	std::vector<Object*> scene;

	//load textures
	gTextures[gTextureHandles::TEST] = loadBmpFile("./res/img/test.bmp");
	gTextures[gTextureHandles::LAMP_POST] = loadBmpFile("./res/img/lampPost.bmp");
	gTextures[gTextureHandles::LAMP_SHADE] = loadBmpFile("./res/img/lampShade.bmp");
	gTextures[gTextureHandles::LAMP_LIGHT] = loadBmpFile("./res/img/lampLight.bmp");
	gTextures[gTextureHandles::PIANO_SHELL] = loadBmpFile("./res/img/pianoShell.bmp");
	gTextures[gTextureHandles::WHITE_KEY] = loadBmpFile("./res/img/whiteKey.bmp");
	gTextures[gTextureHandles::BLACK_KEY] = loadBmpFile("./res/img/blackKey.bmp");
	gTextures[gTextureHandles::SKYBOX_HOR] = loadBmpFile("./res/img/skyboxSideStars.bmp");
	gTextures[gTextureHandles::FLOOR] = loadBmpFile("./res/img/floor.bmp");
	gTextures[gTextureHandles::CEMENT] = loadBmpFile("./res/img/cementBrick.bmp");
	gTextures[gTextureHandles::NOTE] = loadBmpFile("./res/img/note.bmp");

	//create the skybox
	skyBox = ModelFactory::fromSkybox();
	skyBox->setTextureHandle(gTextureHandles::SKYBOX_HOR);

	//create the piano
	Piano* piano = new Piano();
	piano->pos[2] = 1.0f;
	scene.push_back(piano);

	//create the lamp
	Lamp* lamp = new Lamp();
	lamp->pos[0] = 6.5f;
	lamp->pos[2] = 2.5f;
	lightPosition[0] = lamp->pos[0];
	lightPosition[2] = lamp->pos[2];
	scene.push_back(lamp);

	//create the ground
	scene.push_back(new Ground());

	//add the notes to the song
	song.addNotesFromCsv("./res/song/skyReprise.csv", piano);

	return scene;
}

/*
	Update the scene before it is drawn.
*/
void update(std::vector<Object*> scene, double deltaTime) {
	song.update(deltaTime);
	for (size_t i = 0; i < scene.size(); i++) {
		scene.at(i)->update(deltaTime, song.getNoteStatuses());
	}
}

/*
	Draw the scene.
*/
void draw(std::vector<Object*> scene) {
	//clear screen
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//enable textures
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glColor3f(1.0f, 1.0f, 1.0f);

	//reset transformations
	glLoadIdentity();
	gCamera.setModelViewMatrix();

	//draw the skybox
	glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
	skyBox->pos[0] = gCamera.getPosX();
	skyBox->pos[1] = gCamera.getPosY();
	skyBox->pos[2] = gCamera.getPosZ();
	skyBox->draw();
	glEnable(GL_DEPTH_TEST);

	//setup lighting
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);

    float ambient[]   = {0.16f, 0.16f, 0.16f, 1.0f};
    float diffuse[]   = {0.9f, 0.9f, 0.9f, 1.0f};
    float specular[]  = {0.0f, 0.0f, 0.0f, 1.0f};

	glLightfv(GL_LIGHT0,GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0,GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0,GL_SPECULAR, specular);
    glLightfv(GL_LIGHT0,GL_POSITION, lightPosition);
    glEnable(GL_LIGHT0);

	//draw the scene
	for (size_t i = 0; i < scene.size(); i++) {
		scene.at(i)->draw();
	}

	//draw the song's notes
	glDisable(GL_LIGHTING);
	song.draw();
}

//
//	ENTRYPOINT
//
int main(int argc, char* argv[]) {
	//initalize sdl2, exit program if initalization fails
	initSDL();

	//try to create a window, gShouldExit is false if creation fails
	createWindow("MidiVis [Sam Jansen, CSCI 4229]", 1024, 768);
	setProjection(4.0f/3.0f);

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
	delete skyBox;
	for (size_t i = 0; i < scene.size(); i++) {
		delete scene.at(i);
	}

	//cleanup window and SLD before exiting
	cleanup();
    return 0;
}
