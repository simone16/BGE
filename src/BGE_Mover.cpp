#include "BGE_Mover.h"

#include <cmath>

BGE_Mover::BGE_Mover() {}

BGE_Mover::~BGE_Mover() {}

void BGE_Mover::update(float Dt) {
    position += speed*Dt;
}

void BGE_Mover::hit(BGE_2DVect origin, float energy) {
    BGE_Object::hit(origin, energy);
    BGE_2DVect effect = position - origin;
    effect.setPolar( std::sqrt(2*energy/getMass()), effect.angle());
    speed += effect;
}

void BGE_Mover::setAsContent(bool content) {
    BGE_Object::setAsContent(content);
    if (content) {
        speed.x = 0;
        speed.y = 0;
    }
}
