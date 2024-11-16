//
// INCLUDES
//

//graphics and windowing libraries
#include <GL/glew.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_opengl.h"

//helpers
#include "./helpers/sdlHelpers.hpp"
#include "./helpers/openGlHelpers.hpp"
#include "./helpers/csvHelpers.hpp"
#include "./helpers/globals.h"

//classes
#include "./classes/model.hpp"

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

double gTranslate = 0;

//all file globals go here and should never be used elsewhere
float theta = 0.0f;
Model pianoShell;
std::vector<Model> keys;
//std::vector<Model> strings;
std::vector<Model> notes;

//
// UPDATE AND DRAW SCENE
//

/*
	Create textures and models for objects in the scene, assign them references.
*/
void buildScene() {
	//build the shell of the piano using an obj file
	gTextures[0] = loadBmpFile("./res/img/pianoTex.bmp");
	pianoShell.setVertexData(loadObjFile("./res/obj/pianoShell.obj"));
	pianoShell.setTextureHandle(0);

	//notes range from A1 to C#7
	std::vector<std::string> pianoLayout = {
		"A1", "A#1", "B1",
		"C2", "C#2", "D2", "D#2", "E2", "F2", "F#2", "G2", "G#2", "A2", "A#2", "B2"
		"C3", "C#3", "D3", "D#3", "E3", "F3", "F#3", "G3", "G#3", "A3", "A#3", "B3",
		"C4", "C#4", "D4", "D#4", "E4", "F4", "F#4", "G4", "G#4", "A4", "A#4", "B4",
		"C5", "C#5", "D5", "D#5", "E5", "F5", "F#5", "G5", "G#5", "A5", "A#5", "B5",
		"C6", "C#6", "D6", "D#6", "E6", "F6", "F#6", "G6", "G#6", "A6", "A#6", "B6",
		"C7", "C#7"
	};

	//create the keys dynamically
	const float keyboardWidth = 6.0f;
	const float keyWidth = keyboardWidth / (float)pianoLayout.size();
	const float keyHeight = 0.2f;
	const float keyDepth = 1.0f;
	std::cout << keyWidth << std::endl;

	gTextures[1] = loadBmpFile("./res/img/whiteKey.bmp");
	gTextures[2] = loadBmpFile("./res/img/blackKey.bmp");

	for (size_t i = 0; i < pianoLayout.size(); i++) {
		Model newKey;
		newKey.pos[0] = -(keyboardWidth * 0.5f) + (i * keyWidth) + (keyWidth * 0.5f);
		newKey.pos[1] = 3.0f + (keyHeight * 0.5f);
		newKey.pos[2] = 1.8f;

		if (pianoLayout[i].find("#") != std::string::npos) {
			//create a black key
			newKey.setTextureHandle(2);
			newKey.setVertexData(createRectangularPrisimData(keyWidth, keyHeight + 0.08f, keyDepth));
			newKey.pos[1] += 0.04f;
		} else {
			//create a white key
			newKey.setTextureHandle(1);
			newKey.setVertexData(createRectangularPrisimData(keyWidth, keyHeight, keyDepth));
		}

		keys.push_back(newKey);
	}

	notes = readNotesCsv("./res/song/skyReprise.csv", pianoLayout, keys);
}


/*
	Update the scene before it is drawn.
*/
void update(double deltaTime) {
	theta += deltaTime / 100.0f;
	theta = fmod(theta, 360.0f);

	for (size_t i = 0; i < notes.size(); i++) {
		//notes.at(i).pos[1] -= deltaTime / 1000.0f;		
	}
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
	glTranslatef(0, gTranslate, -10);
	//glRotatef(theta, 1.0f, 0.0f, 0.0f);
	//glRotatef(10, 1.0f, 0.0f, 0.0f);


    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	pianoShell.draw();

	for (size_t i = 0; i < keys.size(); i++) {
		keys.at(i).draw();
	}

	for (size_t i = 0; i < notes.size(); i++) {
		notes.at(i).draw();
	}

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