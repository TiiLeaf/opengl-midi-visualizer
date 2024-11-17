#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <GL/glew.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_opengl.h"

#include <vector>

class Model {
    private:
        size_t _vertexCount;
        std::vector<float> _vertexData;
        unsigned int _textureHandle = -1;
        const size_t _stride = 8;

    public:
        float pos[3] = {0, 0, 0};

        size_t getVertexCount() {
            return _vertexCount;
        }

        void setTextureHandle(unsigned int textureHandle) {
            _textureHandle = textureHandle;
        }

        void setVertexData(std::vector<float> vertexData) {
            _vertexData = vertexData;
            _vertexCount = vertexData.size() / _stride;
        }

        void draw() {
            glPushMatrix();

            glTranslatef(pos[0], pos[1], pos[2]);

            glBindTexture(GL_TEXTURE_2D, gTextures[_textureHandle]);

            glBegin(GL_TRIANGLES);
            for (size_t i = 0; i < _vertexData.size(); i += _stride) {
                glTexCoord2f(_vertexData[i + 3], _vertexData[i + 4]);
                glNormal3f(_vertexData[i + 5], _vertexData[i + 6], _vertexData[i + 7]);
                glVertex3f(_vertexData[i], _vertexData[i + 1], _vertexData[i + 2]);
            }
            glEnd();

            glPopMatrix();
        }
};

#endif