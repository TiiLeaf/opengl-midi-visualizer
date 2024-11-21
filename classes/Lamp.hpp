#ifndef LAMP_HPP
#define LAMP_HPP

#include <GL/glew.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_opengl.h"

#include "./Model.hpp"
#include "./ModelFactory.hpp"
#include "../helpers/globals.h"

#include <vector>

class Lamp : public Object {
    private:

    public:
        Lamp() {
            Model* lampPost = ModelFactory::fromLampPost(1.2f, 0.25f, 0.125f, 6.0f);
            lampPost->setTextureHandle(gTextureHandles::LAMP_POST);
            
            Model* lampShade = ModelFactory::fromLampShade(0.8f, 0.6f);
            lampShade->setTextureHandle(gTextureHandles::LAMP_SHADE);
            lampShade->pos[1] = 6.0f;

            Model* lampLight = ModelFactory::fromLampBulb(0.15f, 0.3f);
            lampLight->setTextureHandle(gTextureHandles::LAMP_LIGHT);
            lampLight->pos[1] = 6.5f;

            _models.push_back(lampPost);
            _models.push_back(lampShade);
            _models.push_back(lampLight);
        }
};

#endif