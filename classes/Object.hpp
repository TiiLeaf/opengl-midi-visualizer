#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "./Model.hpp"

/*
    Abstract class that allows me to type every object in the scene the same so I can pack them together into datastructures.
*/
class Object {
    private:
        Model _model;

    public:
        Object(Model model) {
            _model = model;
        }

        virtual void draw() {
            _model.draw();
        }

        virtual void update() = 0;
};

#endif OBJECT_HPP