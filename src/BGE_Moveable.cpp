#include "BGE_Moveable.h"

#include <cmath>

const float BGE_Moveable::SPEED_BOTTOM_LIMIT = 10;

BGE_Moveable::BGE_Moveable(Type objectType, Material objMaterial):
        BGE_Object(objectType, objMaterial) {
    damping = 0.95;
}

BGE_Moveable::~BGE_Moveable() {}

void BGE_Moveable::update(float Dt) {
    position += speed*Dt;
    speed = speed*(1-Dt*damping);
    if (speed.modulus() < SPEED_BOTTOM_LIMIT) {
		speed.x = 0;
		speed.y = 0;
    }
}

void BGE_Moveable::wallBounce(BGE_Object* other, BGE_2DVect overlap) {
    float damage;
    //Correct position.
    position += overlap;
    //Invert speed along collision axis.
    if (std::abs(overlap.x) > std::abs(overlap.y)) {
        speed.x = - speed.x;
        damage = 2*speed.x*speed.x*getMass()/1000;
    }
    else {
        speed.y = - speed.y;
        damage = 2*speed.y*speed.y*getMass()/1000;
    }
    //Apply damage
    health -= damage;
    other->health -= damage;
}

void BGE_Moveable::elasticBounce(BGE_Moveable* other, BGE_2DVect overlap) {
    float damage1, damage2;
    //Correct both positions 50%.
    position += overlap*0.5;
    other->position -= overlap*0.5;
    //Elastic collision.
    //1 is this, 2 is other.
    float m1 = getMass();
    float m2 = other->getMass();
    if (std::abs(overlap.x) > std::abs(overlap.y)) {
        float v1 = speed.x;
        float v2 = other->speed.x;
        speed.x = ((m1-m2)*v1+2*m2*v2)/(m1+m2);
        other->speed.x = ((m2-m1)*v2+2*m1*v1)/(m1+m2);
        damage1 = 2*(speed.x-v1)*(speed.x-v1)*m1/1000;
        damage2 = 2*(other->speed.x-v2)*(other->speed.x-v2)*m2/1000;
    }
    else {
        float v1 = speed.y;
        float v2 = other->speed.y;
        speed.y = ((m1-m2)*v1+2*m2*v2)/(m1+m2);
        other->speed.y = ((m2-m1)*v2+2*m1*v1)/(m1+m2);
        damage1 = 2*(speed.y-v1)*(speed.y-v1)*m1/1000;
        damage2 = 2*(other->speed.y-v2)*(other->speed.y-v2)*m2/1000;
    }
    //Apply damage
    health -= damage1;
    other->health -= damage2;
}

void BGE_Moveable::stick(BGE_Object* other, BGE_2DVect overlap) {
    //Stop upon collision.
    speed.x = 0;
    speed.y = 0;
}
