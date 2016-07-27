// Definition for this class
#include "BGE_Engine.h"

// Definitions of other classes in the project
#include "BGE_Engine.h"

#include <vector>

// Contains global preprocessor defines.
#include <flags.h>

const float BGE_Object::SPEED = 240;
BGE_Engine *BGE_Object::engine = NULL;

BGE_Object::BGE_Object() {
	//Position and speed are initialised to 0,0.
	//Direction initialisation.
	angle = 0;
	//Initialise texture
	texture = NULL;
	//Initialise direction (facing right)
	flip = SDL_FLIP_NONE;
}

BGE_Object::~BGE_Object() {}

void BGE_Object::move( float Dt, std::vector<BGE_Object *> others ) {
	position = position + (speed * Dt);

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

void BGE_Object::render() {
	texture->render( position.x, position.y, flip, angle*57.29577951);
}

bool BGE_Object::circularCollision(BGE_Object* other) {
    return (position - other->getPosition()).modulus() <= (colliderRadius + other->getCollisionRadius());
}

int BGE_Object::getCollisionRadius() {
	return colliderRadius;
}

bool BGE_Object::boxCollision( BGE_Object *other, BGE_2DVect *collisionPoint, BGE_2DVect *correction ) {
	std::vector<BGE_2DVect> thisBox = getCollisionBox();
	std::vector<BGE_2DVect> otherBox = other->getCollisionBox();

	// Adjust correction so that it can be directly applied to this object's position (+)
	// Or other object's position (-)
	if (hasPointsInside( thisBox, otherBox, collisionPoint, correction )) {
        if (correction != NULL) {
			*correction = *correction*(-1.0);
        }
        return true;
	}
	else if (hasPointsInside( otherBox, thisBox, collisionPoint, correction )) {
		return true;
	}
	return false;
}

std::vector<BGE_2DVect> BGE_Object::getCollisionBox() {
    std::vector<BGE_2DVect> corners (4);
    //Calculate corners at angle=0, position=0,0
    // NOTE: consecutive corners must be on the same side of the rectangle!!
    //Upper left
    corners[0].x = - colliderWidth/2;
    corners[0].y = - colliderHeight/2;
    //Upper right
    corners[1].x = + colliderWidth/2;
    corners[1].y = - colliderHeight/2;
    //Bottom right
    corners[2].x = + colliderWidth/2;
    corners[2].y = + colliderHeight/2;
    //Bottom left
    corners[3].x = - colliderWidth/2;
    corners[3].y = + colliderHeight/2;

    for (Uint8 i=0; i<4; i++) {
		//Rotate corners
		float _modulus, _angle;
		_modulus = corners[i].modulus();
		_angle = corners[i].angle();
        corners[i].setPolar(_modulus, _angle+angle);
        //Translate to position
		corners[i] = corners[i]+position;
	}

	return corners;
}

bool BGE_Object::hasPointsInside(const std::vector<BGE_2DVect> &rectangularRegion, const std::vector<BGE_2DVect> &pointsToCheck, BGE_2DVect *collisionPoint, BGE_2DVect *correction) {
	bool pointIsInside = false;

	//If two consecutive points have an identical coordinate => angle = pi/2*N (can't calculate m)
	if (rectangularRegion[1].x == rectangularRegion[2].x || rectangularRegion[1].x == rectangularRegion[0].x ) {
		//Sides of rectangularRegion (assumes consecutive vertices are consecutive)
		float left,right,top,bottom;
		if (rectangularRegion[0].x < rectangularRegion[2].x) {
			left = rectangularRegion[0].x;
			right = rectangularRegion[2].x;
		}
		else {
			left = rectangularRegion[2].x;
			right = rectangularRegion[0].x;
		}
		if (rectangularRegion[0].y < rectangularRegion[2].y) {
			top = rectangularRegion[2].y;
			bottom = rectangularRegion[0].y;
		}
		else {
			top = rectangularRegion[2].y;
			bottom = rectangularRegion[0].y;
		}

		//For each point in pointsToCheck, checks whether it is inside rectangularRegion.
		for (Uint8 i=0; i<4; i++) {
			if ( pointsToCheck[i].x <= right &&
					pointsToCheck[i].x >= left &&
					pointsToCheck[i].y <= top &&
					pointsToCheck[i].y >= bottom ) {
				if (collisionPoint != NULL) {
					// Detects nearest edge, collision point is on this edge and on edgeToPoint distance.
                    if (right - pointsToCheck[i].x < pointsToCheck[i].x - left) {
						if (top - pointsToCheck[i].y < pointsToCheck[i].y - bottom) {
							if (right - pointsToCheck[i].x < top - pointsToCheck[i].y) {
								collisionPoint->x = right;
								collisionPoint->y = pointsToCheck[i].y;
							}
							else {
								collisionPoint->x = pointsToCheck[i].x;
								collisionPoint->y = top;
							}
						}
						else {
							if (right - pointsToCheck[i].x < pointsToCheck[i].y - bottom) {
								collisionPoint->x = right;
								collisionPoint->y = pointsToCheck[i].y;
							}
							else {
								collisionPoint->x = pointsToCheck[i].x;
								collisionPoint->y = bottom;
							}
						}
                    }
                    else {
						if (top - pointsToCheck[i].y < pointsToCheck[i].y - bottom) {
							if (pointsToCheck[i].x - left < top - pointsToCheck[i].y) {
								collisionPoint->x = left;
								collisionPoint->y = pointsToCheck[i].y;
							}
							else {
								collisionPoint->x = pointsToCheck[i].x;
								collisionPoint->y = top;
							}
						}
						else {
							if (pointsToCheck[i].x - left < pointsToCheck[i].y - bottom) {
								collisionPoint->x = left;
								collisionPoint->y = pointsToCheck[i].y;
							}
							else {
								collisionPoint->x = pointsToCheck[i].x;
								collisionPoint->y = bottom;
							}
						}
                    }
					*correction = pointsToCheck[i] - *collisionPoint;
				}
				pointIsInside = true;
				break;
			}
		}
	}
	else {
		//y=mx+q; for each side of rectangularRegion.
		float m[4],q[4];
		for (Uint8 i=0; i<4; i++) {
			Uint8 j = i+1;
			if (j>=4) {
				j=0;
			}
			m[i] = (rectangularRegion[i].y-rectangularRegion[j].y)/(rectangularRegion[i].x-rectangularRegion[j].x);
			q[i] = rectangularRegion[i].y - rectangularRegion[i].x*m[i];
		}

		//Arranges sides 0 and 1 to be respectively higher (y) than 2 and 3.
		if (q[0] < q[2]) {
			float tmp = q[2];
			q[2] = q[0];
			q[0] = tmp;
		}
		if (q[1] < q[3]) {
			float tmp = q[3];
			q[3] = q[1];
			q[1] = tmp;
		}

		//For each point in pointsToCheck, checks whether it is inside rectangularRegion.
		for (Uint8 i=0; i<4; i++) {
			if ( (pointsToCheck[i].y <= pointsToCheck[i].x*m[0]+q[0] &&
					pointsToCheck[i].y >= pointsToCheck[i].x*m[2]+q[2] &&
					pointsToCheck[i].y <= pointsToCheck[i].x*m[1]+q[1] &&
					pointsToCheck[i].y >= pointsToCheck[i].x*m[3]+q[3]) ) {
				pointIsInside = true;
				break;
			}
		}
	}
	return pointIsInside;
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
