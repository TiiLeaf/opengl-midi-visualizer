#ifndef SLD_HELPERS_HPP
#define SLD_HELPERS_HPP

#include <GL/glew.h>
#include "SDL2/SDL.h"

#include "./globals.h"

#include <iostream>

/*
	Initalize SDL2, exit program with code 1 on failure.
*/
void initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cout << "Failed to initalize SDL2: " << SDL_GetError() << std::endl;
		exit(1);
	}
	std::cout << "Initalized SDL." << std::endl;
}

/*
	Create a window and OpenGL rendering context.
*/
void createWindow(const char* title, unsigned short width, unsigned short height) {
	//use OpenGL version 3.3
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	//double buffered window
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	//more accurate depth buffering
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	//create the window
	gWindow = SDL_CreateWindow(
		title,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		width,
		height,
		SDL_WINDOW_OPENGL
	);

	//tell the program to exit gracefully if the window couldn't be created
	if (gWindow == nullptr) {
		std::cout << "Failed to create window: " << SDL_GetError() << std::endl;
		gShouldExit = true;
		return;
	}

	//assign opengl context
	gCtx = SDL_GL_CreateContext(gWindow);

	//tell the program to exit gracefully if the rendering context couldn't be created
	if (gCtx == nullptr) {
		std::cout << "Failed to create rendering context: " << SDL_GetError() << std::endl;
		gShouldExit = true;
		return;
	}

	//initalize glew
	if (glewInit() != GLEW_OK) {
		std::cout << "Could not initalize GLEW!" << std::endl;
		exit(1);
	}

	//print some opengl config info
	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
}

/*
	Free any SDL or OpenGl assets and destroy the window.
*/
void cleanup() {
	//free open gl resources
	glDeleteTextures(gNumTextures, gTextures);

	//cleanup sdl
	std::cout << "Cleaning up SDL." << std::endl;
	SDL_DestroyWindow(gWindow);
	SDL_Quit();
}

/*
	Handle keyboard and window events.
*/
void processEvents(double& camDeltaTheta, double& camDeltaY) {
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		switch (e.type) {
			case SDL_QUIT: //exit button pressed
				gShouldExit = true;
				break;
		}
	}

	const Uint8* keyboardState = SDL_GetKeyboardState(nullptr);
	if (keyboardState[SDL_SCANCODE_ESCAPE]) {
		gShouldExit = true;
	}
	if (keyboardState[SDL_SCANCODE_UP]) {
		camDeltaY += 0.01;
	}
	if (keyboardState[SDL_SCANCODE_DOWN]) {
		camDeltaY -= 0.01;
	}
	if (keyboardState[SDL_SCANCODE_LEFT]) {
		camDeltaTheta -= 0.0025;
	}
	if (keyboardState[SDL_SCANCODE_RIGHT]) {
		camDeltaTheta += 0.0025;
	}
}

#endif