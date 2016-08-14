#include "BGE_Item.h"

#include <BGE_Texture.h>
#include "BGE_Engine.h"

#include <cmath>
#include <stdio.h>
#include <algorithm>	//used for std::find

const float BGE_Item::DRAG = 0.5;
const float BGE_Item::STOP_THRESHOLD = 10;  // [px/S]

BGE_Item::BGE_Item() {
    //Position and speed are initialised to 0,0.
	//Direction initialisation.
	angle = 0;
    setCollision(true);
	//Initialise texture
	texture = NULL;
	//Initialise direction (facing right)
	flip = SDL_FLIP_NONE;

}

BGE_Item::~BGE_Item() {}

void BGE_Item::update(float Dt) {
    position += speed*Dt;
    speed = speed*(1-Dt*DRAG);
    if (speed.modulus() < STOP_THRESHOLD) {
		speed.x = 0;
		speed.y = 0;
    }
}

void BGE_Item::interact(BGE_Object* other, BGE_2DVect overlap) {
    //If other is a tile...
    if (other->type == BGE_Object::TILE) {
        //Correct position.
        position += overlap;
        //Invert speed along collision axis.
        if (std::abs(overlap.x) > std::abs(overlap.y)) {
            speed.x = - speed.x;
        }
        else {
            speed.y = - speed.y;
        }
    }
    //Other is not a tile.
    else {
        BGE_Item *otherMover = static_cast<BGE_Item *>(other);
        //Correct both position 50%.
        position += overlap*0.5;
        otherMover->position -= overlap*0.5;
        //Elastic collision.
        //1 is this, 2 is other.
        float m1 = getMass();
        float m2 = otherMover->getMass();
        if (std::abs(overlap.x) > std::abs(overlap.y)) {
            float v1 = speed.x;
            float v2 = otherMover->speed.x;
            speed.x = ((m1-m2)*v1+2*m2*v2)/(m1+m2);
            otherMover->speed.x = ((m2-m1)*v2+2*m1*v1)/(m1+m2);
        }
        else {
            float v1 = speed.y;
            float v2 = otherMover->speed.y;
            speed.y = ((m1-m2)*v1+2*m2*v2)/(m1+m2);
            otherMover->speed.y = ((m2-m1)*v2+2*m1*v1)/(m1+m2);
        }
    }
}

void BGE_Item::hit(BGE_2DVect origin, float energy) {
    BGE_Object::hit(origin, energy);
    BGE_2DVect effect = position - origin;
    effect.setPolar( std::sqrt(2*energy/getMass()), effect.angle());
    speed += effect;
}

void BGE_Item::die() {
    for (int i=0; i<content.size(); i++) {
        content[i]->position = position;
        content[i]->speed.setPolar(engine->getNormalRandom(100, 50), engine->getRandomFloat(0, TWO_PI));
        content[i]->setAsContent(false);
    }
    content.clear();
    BGE_Object::die();
}


void BGE_Item::setAsContent(bool content) {
    visible = !content;
    solid = !content;
    setCollision( !content);
    if (content) {
        speed.x = 0;
        speed.y = 0;
    }
}

void BGE_Item::add( BGE_Item *item) {
	content.push_back(item);
	item->setAsContent(true);
}

void BGE_Item::remove( BGE_Item *item) {
	std::vector<BGE_Item *>::iterator itemIndex;
	itemIndex = std::find(content.begin(), content.end(), item);
	if ( itemIndex != content.end() ) {
		item->setAsContent(false);
		content.erase( itemIndex);
	}
}

void BGE_Item::render() {
    int col = static_cast<int>(type);
    texture->renderSprite(position.x, position.y, col, 0, flip,  angle*DEGREE_OVER_RADIANS);
}

void BGE_Item::setAngle(float _angle) {
    BGE_Object::setAngle(_angle);
    if (angle > TWO_PI/4 && angle < TWO_PI*3/4) {
        flip = SDL_FLIP_VERTICAL;
    }
    else {
        flip = SDL_FLIP_NONE;
    }
}
