// Definition for this class
#include "BGE_Object.h"

// Definitions of other classes in the project
#include "BGE_Engine.h"

#include <vector>
#include <cmath>
#include <algorithm>	//used for std::find

// Contains global preprocessor defines.
#include <flags.h>

const float BGE_Object::SPEED = 240;
BGE_Engine *BGE_Object::engine = NULL;
const float BGE_Object::TWO_PI = 6.283185307;
const float BGE_Object::DEGREE_OVER_RADIANS = 57.29577951;

BGE_Object::BGE_Object() : content(0) {
	//Initialise interactions.
	visible = true;
	solid = true;
	//Position and speed are initialised to 0,0.
	//Direction initialisation.
	angle = 0;
	//Initialise collider:
	colliderHeight = 0;
	colliderWidth = 0;
	setCollision(true);
	//Initialise texture
	texture = NULL;
	//Initialise direction (facing right)
	flip = SDL_FLIP_NONE;
}

BGE_Object::~BGE_Object() {}

void BGE_Object::applySpeed(float Dt) {
	position = position + (speed * Dt);
}

void BGE_Object::applyCollision(std::vector<BGE_Object*> &others) {
	for ( int i = 0; i < others.size(); i++ ) {
		if ( this != others[i] ) {
			if (circularCollision(others[i])) {
#ifdef DEBUG_COLLISION
				printf("circular collision: %s\n", others[i]->type.getName().c_str());
#endif // DEBUG
				// Checks and avoids overlapping if both objs are solid.
				BGE_2DVect overlap;
				if ( boxCollision(others[i], &overlap)) {
#ifdef DEBUG_COLLISION
					printf("box collision: %s\n", others[i]->type.getName().c_str());
#endif // DEBUG
					interact(others[i], overlap);
				}
			}
		}
	}

	//Keeps the object inside the window (temporary).
	//If the dot went too far to the left or right
	if( position.x < colliderWidth / 2 ) {
		position.x = colliderWidth / 2;
	}
	else if( position.x > BGE_Engine::SCREEN_WIDTH - colliderWidth / 2 ) {
		position.x = BGE_Engine::SCREEN_WIDTH - colliderWidth / 2;
	}

	//If the dot went too far up or down
	if( position.y < colliderHeight / 2 ) {
		position.y = colliderHeight / 2;
	}
	else if( position.y > BGE_Engine::SCREEN_HEIGHT - colliderHeight / 2 ) {
		position.y = BGE_Engine::SCREEN_HEIGHT - colliderHeight / 2;
	}
}

void BGE_Object::setAsContent(bool content) {
    visible = !content;
    solid = !content;
    setCollision( !content);
    if (content) {
		speed.x = 0;
		speed.y = 0;
    }
}

void BGE_Object::add( BGE_Object *object) {
	content.push_back(object);
	object->setAsContent(true);
}

void BGE_Object::remove( BGE_Object *object) {
	std::vector<BGE_Object *>::iterator objectIndex;
	objectIndex = std::find(content.begin(), content.end(), object);
	if ( objectIndex != content.end() ) {
		object->setAsContent(false);
		content.erase( objectIndex);
	}
}

void BGE_Object::render() {
	texture->render( position.x, position.y, flip, angle*DEGREE_OVER_RADIANS);

}

bool BGE_Object::circularCollision( BGE_Object* other) {
	if (!collides || !other->canCollide()) {
		return false;
	}
	else {
		return (position - other->getPosition()).modulus() <= (getCollisionRadius() + other->getCollisionRadius());
	}
}

int BGE_Object::getCollisionRadius() {
	return (std::sqrt( std::pow(colliderWidth, 2) + std::pow(colliderHeight, 2) )/2 + CIRCULAR_COLLISION_PADDING);
}

bool BGE_Object::boxCollision( BGE_Object *other, BGE_2DVect *correction) {
	BGE_2DRect thisBox = getCollisionBox();
	BGE_2DRect otherBox = other->getCollisionBox();

    if (thisBox.overlaps(otherBox)) {
		if (correction != NULL) {
			correction->x = 0;
			correction->y = 0;
			BGE_2DVect otherPosition = other->getPosition();
			BGE_2DRect intersection = thisBox.intersection( otherBox);

			if (intersection.w < intersection.h) {
				if (position.x < otherPosition.x) {
					correction->x = -intersection.w;
				}
				else {
					correction->x = intersection.w;
				}
			}
			else {
				if (position.y < otherPosition.y) {
					correction->y = -intersection.h;
				}
				else {
					correction->y = intersection.h;
				}
			}
		}
		return true;
	}
	return false;
}

BGE_2DRect BGE_Object::getCollisionBox() {
	BGE_2DRect collisionBox ( position.x-colliderWidth/2, position.y-colliderHeight/2, colliderWidth, colliderHeight);
	return collisionBox;
}

void BGE_Object::setTexture( BGE_Texture *ntexture ) {
	texture = ntexture;
}

void BGE_Object::setPosition( float x, float y ) {
	position.x = x;
	position.y = y;
}

BGE_2DVect BGE_Object::getPosition() {
	return position;
}

void BGE_Object::setSpeed( float x, float y ) {
	speed.x = x;
	speed.y = y;

	if ( speed.x > 0 ) {
		flip = SDL_FLIP_NONE;
	}
	else {
		flip = SDL_FLIP_HORIZONTAL;
	}
}

BGE_2DVect BGE_Object::getSpeed() {
	return speed;
}

void BGE_Object::setAngle(float _angle) {
	while (_angle > TWO_PI) {
		_angle -= TWO_PI;
	}
	while (_angle < 0) {
		_angle += TWO_PI;
	}
	angle = _angle;
}

void BGE_Object::setCollision(bool _collides) {
    collides = _collides;
}

bool BGE_Object::canCollide() {
	return collides;
}

bool BGE_Object::isVisible() {
	return visible;
}

bool BGE_Object::isSolid() {
	return solid;
}
