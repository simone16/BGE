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
//Assumes 25px -> 0.5m
const float BGE_Object::LITERS_PER_CUBIC_PX = 1/(8*156.25);
BGE_Object::TypeData BGE_Object::dataOf[TOT];
BGE_Object::MaterialData BGE_Object::dataOfMaterial[static_cast<int>(BGE_Object::Material::TOT)];

void BGE_Object::init() {
    //                  Name        Use          Z_Depth, Width, Height
    dataOf[BARREL] =    {"Barrel",Use::NONE          ,30,25,25};
    dataOf[BLOCK] =     {"Block",Use::NONE          ,25,25,25};
    dataOf[BOTTLE] =    {"Bottle",Use::WEAPON        ,20,10,10};
    dataOf[BULLETS] =   {"Bullets",Use::NONE          ,10,10,10};
    dataOf[CHAIR] =     {"Chair",Use::NONE           ,50,25,25};
    dataOf[GUN] =       {"Gun",Use::SHOOTING_WEAPON,20,15,10};
    dataOf[CREATURE] =  {"Creature",Use::FOOD          ,50,25,25};
    dataOf[MOUSE] =     {"Mouse",Use::FOOD          ,10,10,10};
    dataOf[KNIFE] =     {"Knife",Use::HANDHELD_WEAPON,10,10,10};
    dataOf[STONE] =     {"Stone",Use::WEAPON        ,10,10,10};
    dataOf[SWORD] =     {"Sword",Use::HANDHELD_WEAPON,10,25,10};
    dataOf[SPLINTERS] = {"Splinters",Use::NONE      ,0,0,0};
    dataOf[TABLE] =     {"Table",Use::NONE           ,25,25,25};
    dataOf[TILE] =      {"Tile",Use::NONE           ,25,25,25};

    //                  		 name       state                     density 					nutrition
    dataOfMaterial[static_cast<int>(Material::FLESH)] =     {"meat",   PhysicalState::SOFT_SOLID   ,1.0*LITERS_PER_CUBIC_PX,1.0};
    dataOfMaterial[static_cast<int>(Material::BONE)] =      {"bone",    PhysicalState::HARD_SOLID   ,0.7*LITERS_PER_CUBIC_PX,0.1};
    dataOfMaterial[static_cast<int>(Material::IRON)] =      {"iron",    PhysicalState::HARD_SOLID   ,7.9*LITERS_PER_CUBIC_PX,-0.5};
    dataOfMaterial[static_cast<int>(Material::PINEWOOD)] =  {"pine wood",PhysicalState::HARD_SOLID  ,0.5*LITERS_PER_CUBIC_PX,-0.1};
    dataOfMaterial[static_cast<int>(Material::MARBLE)] =    {"marble",  PhysicalState::HARD_SOLID   ,2.7*LITERS_PER_CUBIC_PX,-1.0};
    dataOfMaterial[static_cast<int>(Material::FIBERGLASS)] = {"fiberglass",PhysicalState::HARD_SOLID,2.0*LITERS_PER_CUBIC_PX,-2.0};
    dataOfMaterial[static_cast<int>(Material::DRUG)] =      {"drugs",   PhysicalState::SOFT_SOLID   ,1.0*LITERS_PER_CUBIC_PX,0.0};
    dataOfMaterial[static_cast<int>(Material::WATER)] =     {"water",   PhysicalState::LIQUID       ,1.0*LITERS_PER_CUBIC_PX,0.5};
    dataOfMaterial[static_cast<int>(Material::FABRIC)] =    {"fabric",  PhysicalState::SOFT_SOLID   ,0.2*LITERS_PER_CUBIC_PX,0.1};
}

BGE_Object::BGE_Object() : content(0) {
	//Position is initialised to 0,0
	//Content is initialised to empty.
	//Set a default type.
	type = BARREL;
	//Initialise interactions.
	visible = true;
	solid = true;
	collides = true;
	//Initialise health.
	health = 3000;
	//Direction initialisation.
	angle = 0;
	//Initialise pointers.
	texture = NULL;
	//Initialise sprite flip (facing right).
	flip = SDL_FLIP_NONE;
	animCtr = 15;
}

BGE_Object::~BGE_Object() {}

void BGE_Object::hit(BGE_2DVect origin, float energy) {
    printf("You hit %s\n", getName().c_str());
	health -= energy;
	if (health <= 0) {
        die();
	}
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
}

void BGE_Object::setAsContent(bool content) {
    visible = !content;
    solid = !content;
    setCollision( !content);
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

void BGE_Object::die() {
    for (int i=0; i<content.size(); i++) {
        content[i]->position = position;
        BGE_Item *item = static_cast<BGE_Item *>(content[i]);
        item->speed.setPolar(engine->getNormalRandom(100, 50), engine->getRandomFloat(0, TWO_PI));
        content[i]->setAsContent(false);
    }
    content.clear();
    engine->remove(this);
}

void BGE_Object::render() {
    if (type == SPLINTERS) {
        texture->renderSprite( position.x, position.y, static_cast<int>(material), 3-animCtr/4);
        animCtr--;
        if (animCtr < 0) {
            engine->remove(this);
        }
    }
    else {
        texture->render( position.x, position.y, flip, angle*DEGREE_OVER_RADIANS);
    }
}

bool BGE_Object::circularCollision( BGE_Object* other) {
	if (!collides || !other->canCollide()) {
		return false;
	}
	else {
		return (position - other->position).modulus() <= (getCollisionRadius() + other->getCollisionRadius());
	}
}

float BGE_Object::getCollisionRadius() {
	return std::sqrt(dataOf[type].width*dataOf[type].width + dataOf[type].height*dataOf[type].height)/2 + CIRCULAR_COLLISION_PADDING;
}

bool BGE_Object::boxCollision( BGE_Object *other, BGE_2DVect *correction) {
	BGE_2DRect thisBox = getCollisionBox();
	BGE_2DRect otherBox = other->getCollisionBox();

    if (thisBox.overlaps(otherBox)) {
		if (correction != NULL) {
			correction->x = 0;
			correction->y = 0;
			BGE_2DRect intersection = thisBox.intersection( otherBox);
			//Correction is the shortest vector which guarantees no overlap
			//if applied to this.
			if (intersection.w < intersection.h) {
				if (position.x < other->position.x) {
					correction->x = -intersection.w;
				}
				else {
					correction->x = intersection.w;
				}
			}
			else {
				if (position.y < other->position.y) {
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
	BGE_2DRect collisionBox (-dataOf[type].width/2, -dataOf[type].height/2, dataOf[type].width, dataOf[type].height);
	collisionBox += position;
	return collisionBox;
}

void BGE_Object::setTexture( BGE_Texture *ntexture ) {
	texture = ntexture;
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

BGE_Object::Use BGE_Object::getUse() {
	return dataOf[type].use;
}

float BGE_Object::getVolume() {
	return dataOf[type].depth * dataOf[type].height * dataOf[type].width;
}

float BGE_Object::getMass() {
    return getVolume() * dataOfMaterial[static_cast<int>(material)].density;
}

float BGE_Object::getNutrition() {
	return getMass() * dataOfMaterial[static_cast<int>(material)].nutrition;
}

BGE_Object::PhysicalState BGE_Object::getPhysicalState() {
	return dataOfMaterial[static_cast<int>(material)].state;
}

std::string BGE_Object::getName() {
    return dataOf[type].name + " of " + dataOfMaterial[static_cast<int>(material)].name;
}
