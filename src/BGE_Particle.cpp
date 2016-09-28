#include "BGE_Particle.h"

#include <BGE_Engine.h>

BGE_Particle::BGE_Particle( Material _material ):
        BGE_Moveable(BGE_Object::PARTICLE, _material) {
    if (getPhysicalState() == PhysicalState::LIQUID) {
        interactBehaviour = &BGE_Particle::stick;
    }
    else {
        interactBehaviour = &BGE_Particle::wallBounce;
    }
}

BGE_Particle::~BGE_Particle() {}


void BGE_Particle::update(float Dt) {
    health -= Dt;
    if (health <= 0) {
        die();
    }
    else {
        BGE_Moveable::update(Dt);
    }
}

void BGE_Particle::interact(BGE_Object* other, BGE_2DVect overlap) {
    (this->*interactBehaviour)(other, overlap);
}

void BGE_Particle::render() {
    SDL_Color color = getColor();
    engine->particlesSheet.setColor( color.r, color.g, color.b);
    engine->particlesSheet.renderSprite( position.x, position.y, 0, 0);
}
