#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "./Model.hpp"
#include <vector>

class Object {
    protected:
        std::vector<Model*> _models;
        
    public:
        float pos[3] = {0, 0, 0};

        virtual ~Object() {
            for (size_t i = 0; i < _models.size(); i++) {
                delete _models.at(i);
            }
        }

        virtual void draw() {
            glPushMatrix();
            glTranslatef(pos[0], pos[1], pos[2]);
            for (size_t i = 0; i < _models.size(); i++) {
                _models.at(i)->draw();
            }
            glPopMatrix();
        }

        virtual void update(double deltaTime, std::vector<int> noteStatuses) {}
};

#endif