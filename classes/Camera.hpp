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
        double _yMin = 0.1;
        double _yMax = 25;

    public:
        Camera(double theta, double y, double r) {
            _theta = theta;
            _y = y;
            _r = r;
        }

        void move(double deltaTheta, double deltaY, double deltaTime) {
            _theta += (deltaTheta * deltaTime);
            _y += (deltaY * deltaTime);
            _y = std::max(_y, _yMin);
            _y = std::min(_y, _yMax);
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

        float getPosX() {
            return sin(_theta) * _r;
        }

        float getPosY() {
            return _y;
        }

        float getPosZ() {
            return cos(_theta) * _r;;
        }
};

#endif