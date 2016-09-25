#include "BGE_Item.h"

#include <BGE_Texture.h>
#include "BGE_Engine.h"

#include <cmath>
#include <stdio.h>
#include <algorithm>	//used for std::find

#include <flags.h>

const float BGE_Item::DRAG = 0.5;
const float BGE_Item::STOP_THRESHOLD = 10;  // [px/S]

BGE_Item::BGE_Item( Type objectType, Material objMaterial):
    BGE_Object(objectType, objMaterial) {
    //Speed is initialised to 0,0.
    //Initialise movement locking.
    lock = MovementLock::NONE;
}

BGE_Item::~BGE_Item() {}

void BGE_Item::update(float Dt) {
    position += speed*Dt;
    speed = speed*(1-Dt*DRAG);
    if (speed.modulus() < STOP_THRESHOLD) {
		speed.x = 0;
		speed.y = 0;
    }
    //Reset lock before interact() is called:
    lock = MovementLock::NONE;
}

void BGE_Item::interact(BGE_Object* other, BGE_2DVect overlap) {
    //If other is a tile...
    if (other->type == BGE_Object::TILE) {
        //Correct position.
        position += overlap;
        //Invert speed along collision axis and set lock.
        if (std::abs(overlap.x) > std::abs(overlap.y)) {
            speed.x = - speed.x;
            if (lock == MovementLock::VERTICAL) {
                lock = MovementLock::BOTH;
            }
            else {
                lock = MovementLock::HORIZONTAL;
            }
        }
        else {
            speed.y = - speed.y;
            if (lock == MovementLock::HORIZONTAL) {
                lock = MovementLock::BOTH;
            }
            else {
                lock = MovementLock::VERTICAL;
            }
        }
    }
    //Other is not a tile.
    else {
        BGE_Item *otherMover = static_cast<BGE_Item *>(other);
        if (otherMover->lock == MovementLock::HORIZONTAL && std::abs(overlap.x) > std::abs(overlap.y)) {
            //Correct position.
            position += overlap;
            speed.x = - speed.x;
            if (lock == MovementLock::VERTICAL) {
                lock = MovementLock::BOTH;
            }
            else {
                lock = MovementLock::HORIZONTAL;
            }
        }
        else if (otherMover->lock == MovementLock::VERTICAL && std::abs(overlap.x) < std::abs(overlap.y)) {
            //Correct position.
            position += overlap;
            speed.y = - speed.y;
            if (lock == MovementLock::HORIZONTAL) {
                lock = MovementLock::BOTH;
            }
            else {
                lock = MovementLock::VERTICAL;
            }
        }
        else {
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
}

void BGE_Item::hit(BGE_Object* origin, float energy) {
    BGE_Object::hit(origin, energy);
    BGE_2DVect effect = position - origin->position;
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
#ifdef INCOMPLETE_SHEETS
    if (dataOf[type].sheet == NULL) {
        engine->itemSheetSmall.renderSprite(position.x, position.y, 0, 0, flip,  angle*DEGREE_OVER_RADIANS);
        return;
    }
#endif // INCOMPLETE_SHEETS
    dataOf[type].sheet->renderSprite(position.x, position.y, dataOf[type].column, 0, flip,  angle*DEGREE_OVER_RADIANS);
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
