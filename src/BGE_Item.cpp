#include "BGE_Item.h"

#include <BGE_Texture.h>

#include <cmath>

const float BGE_Item::DRAG = 0.5;
const float BGE_Item::STOP_THRESHOLD = 10;

BGE_Item::BGE_Item() {
    //Position and speed are initialised to 0,0.
	//Direction initialisation.
	angle = 0;
	//Initialise collider:
	colliderHeight = 25;
	colliderWidth = 25;
    setCollision(true);
	//Initialise texture
	texture = NULL;
	//Initialise direction (facing right)
	flip = SDL_FLIP_NONE;

	//Set object properties.
	BGE_ObjectMaterial material;
	material.material = BGE_ObjectMaterial::Material::PINEWOOD;
	material.state = BGE_ObjectMaterial::PhysicalState::HARD_SOLID;
	type.type = BGE_ObjectType::Type::BARREL;
    type.material = material;
}

BGE_Item::~BGE_Item() {}

void BGE_Item::applySpeed(float Dt) {
    BGE_Object::applySpeed(Dt);
    speed = speed*(1-Dt*DRAG);
    if (speed.modulus() < STOP_THRESHOLD) {
		speed.x = 0;
		speed.y = 0;
    }
}

void BGE_Item::interact(BGE_Object* other, BGE_2DVect overlap) {
    //Apply overlap:
    BGE_2DVect otherSpeed = other->getSpeed();

    //If at least one is moving.
    if (speed.x != 0 || speed.y != 0 || otherSpeed.x != 0 || otherSpeed.y != 0) {
        //If this is not supposed to move.
        if (!type.canMove()) {
            //Invert other speed along collision axis.
            if (std::abs(overlap.x) > std::abs(overlap.y)) {
                other->setSpeed(-otherSpeed.x, otherSpeed.y);
            }
            else {
                other->setSpeed(otherSpeed.x, -otherSpeed.y);
            }
        }
        //If other is not supposed to move.
        else if (!other->type.canMove()) {
            //Invert speed along collision axis.
            if (std::abs(overlap.x) > std::abs(overlap.y)) {
                speed.x = - speed.x;
            }
            else {
                speed.y = - speed.y;
            }
        }
        else {
            //Elastic collision.
            BGE_2DVect newPosition = other->getPosition() - overlap*0.5;
            other->setPosition(newPosition.x, newPosition.y);
            position = position + overlap*0.5;
            //1 is this, 2 is other.
            float m1 = type.getMass();
            float m2 = other->type.getMass();
            if (overlap.x > overlap.y) {
                float v1 = speed.x;
                float v2 = otherSpeed.x;
                float nv1 = ((m1-m2)*v1+2*m2*v2)/(m1+m2);
                float nv2 = ((m2-m1)*v2+2*m1*v1)/(m1+m2);
                other->setSpeed(nv2, otherSpeed.y);
                speed.x = nv1;
            }
            else {
                float v1 = speed.y;
                float v2 = otherSpeed.y;
                float nv1 = ((m1-m2)*v1+2*m2*v2)/(m1+m2);
                float nv2 = ((m2-m1)*v2+2*m1*v1)/(m1+m2);
                other->setSpeed(otherSpeed.x, nv2);
                speed.y = nv1;
            }
        }
    }
}

void BGE_Item::render() {
    int col,row;
    row = 0;
    switch (type.type) {
        case BGE_ObjectType::Type::BARREL:
            col = 0;
            break;
        case BGE_ObjectType::Type::KNIFE:
            col = 1;
            break;
        case BGE_ObjectType::Type::STONE:
            col = 2;
            break;
        case BGE_ObjectType::Type::BLOCK:
            col = 3;
            break;
        case BGE_ObjectType::Type::BULLETS:
            col = 0;
            row = 1;
            break;
        case BGE_ObjectType::Type::GUN:
            col = 1;
            row = 1;
            break;
        default:
            col = 0;
            break;
    }
    texture->renderSprite(position.x, position.y, col, row, flip,  angle*BGE_Object::DEGREE_OVER_RADIANS);
}
