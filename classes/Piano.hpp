#ifndef PIANO_HPP
#define PIANO_HPP

#include <GL/glew.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_opengl.h"

#include "./Model.hpp"
#include "../helpers/globals.h"

#include <vector>
#include <string>

class Piano : public Object {
    private:
        std::vector<Model*> _keys;

    public:
        ~Piano() {
            for (size_t i = 0; i < _keys.size(); i++) {
                delete _keys.at(i);
            }
            
            Object::~Object();
        }

        Piano(Model* model) : Object(model) {
            std::vector<std::string> pianoLayout = {
                "A1", "A#1", "B1",
                "C2", "C#2", "D2", "D#2", "E2", "F2", "F#2", "G2", "G#2", "A2", "A#2", "B2"
                "C3", "C#3", "D3", "D#3", "E3", "F3", "F#3", "G3", "G#3", "A3", "A#3", "B3",
                "C4", "C#4", "D4", "D#4", "E4", "F4", "F#4", "G4", "G#4", "A4", "A#4", "B4",
                "C5", "C#5", "D5", "D#5", "E5", "F5", "F#5", "G5", "G#5", "A5", "A#5", "B5",
                "C6", "C#6", "D6", "D#6", "E6", "F6", "F#6", "G6", "G#6", "A6", "A#6", "B6",
                "C7", "C#7"
            };

            //create the keys dynamically based on `pianoLayout`
            const float keyboardWidth = 8.0f;
            const size_t keyCount = pianoLayout.size();
            const float keyWidth = keyboardWidth / keyCount;
            const float keyHeight = 0.18f;
            const float keyDepth = 0.8f;

            for (size_t i = 0; i < keyCount; i++) {
                if (pianoLayout.at(i).find('#') != std::string::npos) {
                    //create a black key
                    Model* newKey = ModelFactory::fromAnchoredCuboid(keyWidth, keyHeight, keyDepth);
                    newKey->setTextureHandle(gTextureHandles::BLACK_KEY);

                    newKey->pos[0] = -(keyboardWidth * 0.5f) + (i * keyWidth);
                    newKey->pos[1] = 3.0f;
                    newKey->pos[2] = 0.0f;
                    _keys.push_back(newKey);
                } else {
                    //create a white key
                    Model* newKey = ModelFactory::fromAnchoredCuboid(keyWidth, keyHeight, keyDepth);
                    newKey->setTextureHandle(gTextureHandles::WHITE_KEY);

                    newKey->pos[0] = -(keyboardWidth * 0.5f) + (i * keyWidth);
                    newKey->pos[1] = 3.0f;
                    newKey->pos[2] = 0.0f;
                    _keys.push_back(newKey);
                }
            }
        }

        void update(float deltaTime) override {}

        void draw() override {
            Object::draw();
            for (size_t i = 0; i < _keys.size(); i++) {
                _keys.at(i)->draw();
            }
        }
};

#endif