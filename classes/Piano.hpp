#ifndef PIANO_HPP
#define PIANO_HPP

#include <GL/glew.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_opengl.h"

#include "./Model.hpp"
#include "./ModelFactory.hpp"
#include "../helpers/globals.h"

#include <vector>
#include <array>
#include <string>

class Piano : public Object {
    private:
        std::vector<std::array<float, 6>> _strings;

    public:
        Piano() {
            std::vector<std::string> pianoLayout = {
                "A1", "A#1", "B1",
                "C2", "C#2", "D2", "D#2", "E2", "F2", "F#2", "G2", "G#2", "A2", "A#2", "B2",
                "C3", "C#3", "D3", "D#3", "E3", "F3", "F#3", "G3", "G#3", "A3", "A#3", "B3",
                "C4", "C#4", "D4", "D#4", "E4", "F4", "F#4", "G4", "G#4", "A4", "A#4", "B4",
                "C5", "C#5", "D5", "D#5", "E5", "F5", "F#5", "G5", "G#5", "A5", "A#5", "B5",
                "C6", "C#6", "D6", "D#6", "E6", "F6", "F#6", "G6", "G#6", "A6", "A#6", "B6",
                "C7", "C#7", "D7"
            };

            std::vector<float> stringLengths = {
                7.2f, 7.2f, 7.2f, 
                7.2f, 7.2f, 7.2f, 7.2f, 7.2f, 7.2f, 7.2f, 7.2f, 7.2f, 7.2f, 7.2f, 7.2f,
                7.0f, 7.0f, 7.0f, 7.0f, 7.0f, 7.0f, 7.0f, 7.0f, 7.0f, 7.0f, 7.0f, 7.0f,
                6.5f, 6.5f, 6.5f, 6.5f, 6.5f, 6.5f, 6.5f, 6.5f, 6.5f, 6.5f, 6.5f, 6.5f,
                3.6f, 3.6f, 3.6f, 3.6f, 3.6f, 3.0f, 2.6f, 2.6f, 2.6f, 2.6f, 2.6f, 2.6f,
                2.4f, 2.4f, 2.4f, 2.4f, 2.4f, 2.2f, 2.2f, 2.2f, 2.2f, 2.2f, 2.2f, 2.2f,
                2.0f, 2.0f, 2.0f
            };

            //create the keys dynamically based on `pianoLayout`
            const float keyboardWidth = 8.0f;
            const size_t keyCount = pianoLayout.size();
            const float whiteKeyWidth = keyboardWidth / (keyCount - 27);
            const float blackKeyWidth = whiteKeyWidth * 0.66;
            const float keyHeight = 0.18f;
            const float keyDepth = 0.8f;
            
            int whiteKeysAdded = 0;
            for (size_t i = 0; i < keyCount; i++) {
                if (pianoLayout.at(i).find('#') != std::string::npos) {
                    //create a black key
                    Model* newKey = ModelFactory::fromBlackKey(blackKeyWidth, keyHeight, keyDepth - 0.15f, 0.15f);
                    newKey->setTextureHandle(gTextureHandles::BLACK_KEY);

                    float x = -(keyboardWidth * 0.5f) + (whiteKeysAdded * whiteKeyWidth) - (blackKeyWidth * 0.5f);
                    float y = 3.08f;
                    newKey->pos[0] = x;
                    newKey->pos[1] = y;
                    _models.push_back(newKey);

                    float stringX = x + (blackKeyWidth * 0.5f);
                    float stringY = y + 0.25f;
                    float stringZ = -stringLengths.at(i);
                    _strings.push_back({stringX, stringY, -0.5f, stringX, stringY, stringZ});
                } else {
                    //create a white key
                    Model* newKey = ModelFactory::fromAnchoredCuboid(whiteKeyWidth, keyHeight, keyDepth);
                    newKey->setTextureHandle(gTextureHandles::WHITE_KEY);

                    float x = -(keyboardWidth * 0.5f) + (whiteKeysAdded * whiteKeyWidth);
                    float y = 3.0f;
                    newKey->pos[0] = x;
                    newKey->pos[1] = y;
                    _models.push_back(newKey);

                    float stringX = x + (whiteKeyWidth * 0.5f);
                    float stringY = y + 0.25f;
                    float stringZ = -stringLengths.at(i);
                    _strings.push_back({stringX, stringY, -0.5f, stringX, stringY, stringZ});

                    whiteKeysAdded += 1;
                }
            }

            Model* shellModel = ModelFactory::fromObj("./res/obj/pianoShell.obj");
            shellModel->setTextureHandle(gTextureHandles::PIANO_SHELL);
            _models.push_back(shellModel);
        }

        void draw() override {
            // Draw the strings
            glDisable(GL_TEXTURE_2D);
            glPushMatrix();
            glTranslatef(pos[0], pos[1], pos[2]);
            glBegin(GL_LINES);
            for (size_t i = 0; i < _strings.size(); i++) {
                glVertex3f(_strings.at(i)[0], _strings.at(i)[1], _strings.at(i)[2]);
                glVertex3f(_strings.at(i)[3], _strings.at(i)[4], _strings.at(i)[5]);
            }
            glEnd();
            glPopMatrix();
            glEnable(GL_TEXTURE_2D);

            // Draw the rest of the models under the infuence of the light
            Object::draw();
        }
};

#endif