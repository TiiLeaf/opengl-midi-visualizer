#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <GL/glew.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_opengl.h"
#include <cmath>

class Camera {
    private:
        double _theta;
        double _y;
        double _r;

    public:
        Camera(double theta, double y, double r) {
            _theta = theta;
            _y = y;
            _r = r;
        }

        void move(double deltaTheta, double deltaY, double deltaTime) {
            _theta += (deltaTheta * deltaTime);
            _y += (deltaY * deltaTime);
        }

        void setModelViewMatrix() {
            double x = sin(_theta) * _r;
            double z = cos(_theta) * _r;
            gluLookAt(
                x, _y, z,
                0, (_y * 0.1) + 2, 0,
                0, 1, 0
            );
        }
};

#endif