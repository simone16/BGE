#include "BGE_Particle.h"

#include <BGE_Engine.h>

#include <cstdio>

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

void BGE_Particle::explosion(BGE_2DVect position, float energy, Material mat, float direction, float width) {
    int particleCount = int(energy/200);
    BGE_Particle part (mat);
    float meanSpeed = std::sqrt( energy/part.getMass());
    if (width == TWO_PI) {
        for (int i=0; i<particleCount; i++) {
            BGE_Particle *particle = new BGE_Particle(mat);
            particle->position = position;
            particle->health = engine->getNormalRandom(energy/4000, energy/5000);
            particle->speed.setPolar( engine->getNormalRandom( meanSpeed, 0.4*meanSpeed),
                                    engine->getRandomFloat(0, TWO_PI));
            engine->add(particle);
        }
    }
    else {
        for (int i=0; i<particleCount; i++) {
            BGE_Particle *particle = new BGE_Particle(mat);
            particle->position = position;
            particle->health = engine->getNormalRandom(energy/4000, energy/5000);
            particle->speed.setPolar( engine->getNormalRandom( meanSpeed, 0.4*meanSpeed),
                                    engine->getNormalRandom( direction, width));
            engine->add(particle);
        }
    }
}

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
