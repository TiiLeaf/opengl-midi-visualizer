#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "./Model.hpp"

/*
    Abstract class that allows me to type every object in the scene the same so I can pack them together into datastructures.
*/
class Object {
    private:
        Model* _model;
        

    public:
        float pos[3] = {0, 0, 0};

        virtual ~Object() {
            delete _model;
        }

        Object(Model* model) {
            _model = model;
        }

        virtual void draw() {
            glPushMatrix();
            glTranslatef(pos[0], pos[1], pos[2]);
            _model->draw();
            glPopMatrix();
        }

        virtual void update(float deltaTime) = 0;
};

#endif