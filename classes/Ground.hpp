#ifndef GROUND_HPP
#define GROUND_HPP

#include <GL/glew.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_opengl.h"

#include "./Model.hpp"
#include "./ModelFactory.hpp"
#include "../helpers/globals.h"

class Ground : public Object {
    private:

    public:
        Ground() {
            Model* ground = ModelFactory::fromTurrets(12, 1); 
            ground->setTextureHandle(gTextureHandles::CEMENT);

            Model* circle = ModelFactory::fromFloor(12);
            circle->setTextureHandle(gTextureHandles::FLOOR);

            _models.push_back(circle);
            _models.push_back(ground);
        }
};

#endif