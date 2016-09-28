// Definition for this class
#include "BGE_Object.h"

// Definitions of other classes in the project
#include "BGE_Engine.h"

#include <vector>
#include <cmath>
#include <algorithm>	//used for std::find

// Contains global preprocessor defines.
#include <flags.h>

BGE_Engine *BGE_Object::engine = NULL;
const float BGE_Object::TWO_PI = 6.283185307;
const float BGE_Object::DEGREE_OVER_RADIANS = 57.29577951;
//Assumes 25px -> 0.5m
const float BGE_Object::LITERS_PER_CUBIC_PX = 1/(8*156.25);
BGE_Object::TypeData BGE_Object::dataOf[TOT];
BGE_Object::MaterialData BGE_Object::dataOfMaterial[static_cast<int>(BGE_Object::Material::TOT)];
BGE_Object::CreatureData BGE_Object::dataOfCreature[static_cast<int>(BGE_Object::CreatureType::TOT)];

void BGE_Object::init() {
    //                  Name        Use            reload_time  base_health    Z_Depth, Width, Height   sprite sheet and column
    dataOf[BARREL] =    {"Barrel",Use::NONE        ,0.1        ,5000            ,30,25,25               ,&(engine->itemSheetTall),2};
    dataOf[BLOCK] =     {"Block",Use::NONE         ,0.1        ,1100            ,25,25,25               ,NULL,0};
    dataOf[BOTTLE] =    {"Bottle",Use::WEAPON      ,0.2        ,100             ,20,10,10               ,&(engine->itemSheetTall),3};
    dataOf[BULLETS] =   {"Bullets",Use::NONE       ,0.1        ,2000            ,10,10,10               ,&(engine->itemSheetSmall),1};
    dataOf[CHAIR] =     {"Chair",Use::NONE         ,0.1        ,5000            ,50,25,25               ,NULL,0};
    dataOf[GUN] =       {"Gun",Use::SHOOTING_WEAPON,0.5        ,2000            ,20,15,10               ,&(engine->itemSheetSmall),2};
    dataOf[CREATURE] =  {"Creature",Use::FOOD      ,1.0        ,0               ,50,25,25               ,NULL,0};
    dataOf[MOUSE] =     {"Mouse",Use::FOOD         ,1.0        ,500             ,10,10,10               ,NULL,0};
    dataOf[KNIFE] =     {"Knife",Use::HANDHELD_WEAPON,0.5      ,2000            ,10,10,10               ,&(engine->itemSheetSmall),3};
    dataOf[STONE] =     {"Stone",Use::WEAPON       ,0.5        ,6000            ,10,10,10               ,&(engine->itemSheetSmall),4};
    dataOf[SWORD] =     {"Sword",Use::HANDHELD_WEAPON,0.5      ,3000            ,10,25,10               ,&(engine->itemSheetTall) ,0};
    dataOf[PARTICLE] =  {"Splinters",Use::NONE     ,0          ,1               ,5,5,5                  ,&(engine->itemSheetSmall),0};
    dataOf[TABLE] =     {"Table",Use::NONE         ,0.1        ,5000            ,25,25,25               ,NULL,0};
    dataOf[TILE] =      {"Tile",Use::NONE          ,0.1        ,100000          ,25,25,25               ,NULL,0};

    //                  		                            name       state                     density 					nutrition   strenght
    dataOfMaterial[static_cast<int>(Material::FLESH)] =     {"meat",   PhysicalState::SOFT_SOLID   ,1.0*LITERS_PER_CUBIC_PX,1.0         ,0.5};
    dataOfMaterial[static_cast<int>(Material::BONE)] =      {"bone",    PhysicalState::HARD_SOLID   ,0.7*LITERS_PER_CUBIC_PX,0.1        ,1.0};
    dataOfMaterial[static_cast<int>(Material::IRON)] =      {"iron",    PhysicalState::HARD_SOLID   ,7.9*LITERS_PER_CUBIC_PX,-0.5       ,3.0};
    dataOfMaterial[static_cast<int>(Material::PINEWOOD)] =  {"pine wood",PhysicalState::HARD_SOLID  ,0.5*LITERS_PER_CUBIC_PX,-0.1       ,2.0};
    dataOfMaterial[static_cast<int>(Material::MARBLE)] =    {"marble",  PhysicalState::HARD_SOLID   ,2.7*LITERS_PER_CUBIC_PX,-1.0       ,3.0};
    dataOfMaterial[static_cast<int>(Material::FIBERGLASS)] = {"fiberglass",PhysicalState::HARD_SOLID,2.0*LITERS_PER_CUBIC_PX,-2.0       ,2.0};
    dataOfMaterial[static_cast<int>(Material::DRUG)] =      {"drugs",   PhysicalState::SOFT_SOLID   ,1.0*LITERS_PER_CUBIC_PX,0.0        ,0.3};
    dataOfMaterial[static_cast<int>(Material::WATER)] =     {"water",   PhysicalState::LIQUID       ,1.0*LITERS_PER_CUBIC_PX,0.5        ,0.1};
    dataOfMaterial[static_cast<int>(Material::FABRIC)] =    {"fabric",  PhysicalState::SOFT_SOLID   ,0.2*LITERS_PER_CUBIC_PX,0.1        ,0.5};
    dataOfMaterial[static_cast<int>(Material::GOLD)] =      {"gold",    PhysicalState::HARD_SOLID   ,19.3*LITERS_PER_CUBIC_PX,0.0       ,1.0};

    //                                                          name            health  viewFld aimAcc       runSp   walkSp   sprite_index
    dataOfCreature[static_cast<int>(CreatureType::COP)] =       {"cop",         3000    ,400    ,TWO_PI*0.02 ,120    ,50      ,0};
    dataOfCreature[static_cast<int>(CreatureType::PLAYER)] =    {"poor bastard",10000   ,400    ,0.0         ,240    ,240     ,1};
    dataOfCreature[static_cast<int>(CreatureType::COWBOY)] =    {"cowboy",      3000    ,400    ,TWO_PI*0.03 ,100    ,40      ,2};
    dataOfCreature[static_cast<int>(CreatureType::SHERLOK)] =   {"detective",   5000    ,200    ,TWO_PI*0.07 ,100    ,30      ,3};
}

BGE_Object::BGE_Object( Type objectType, Material objMaterial ) {
	//Position is initialised to 0,0
	//Content is initialised to empty.
    type = objectType;
    material = objMaterial;
	health = getMaxHealth();
	//Initialise interactions.
	visible = true;
	solid = true;
	collides = true;
	//Direction initialisation.
	angle = 0;
	//Initialise messaging.
	messageTexture = NULL;
	messageTimer = 0;
	//Initialise sprite flip (facing right).
	flip = SDL_FLIP_NONE;
	animCtr = 15;//TODO porcodio che schifezza fai una classe per gli effetti.
}

BGE_Object::~BGE_Object() {
    if (messageTexture != NULL) {
        delete messageTexture;
        messageTexture = NULL;
    }
}

void BGE_Object::update(float Dt) {
    if (messageTexture != NULL) {
        messageTimer -= Dt;
        if (messageTimer < 0) {
            delete messageTexture;
            messageTexture = NULL;
        }
    }
}

void BGE_Object::hit(BGE_Object* origin, float energy) {
#ifdef DEBUG
    printf("%s hit %s\n", origin->getName().c_str(), getName().c_str());
#endif // DEBUG
	health -= energy;
	if (health <= 0) {
        die();
	}
}

void BGE_Object::applyCollision(std::vector<BGE_Object*> &others) {
	for ( int i = 0; i < others.size(); i++ ) {
		if ( this != others[i] ) {
			if (circularCollision(others[i])) {
				// Checks and avoids overlapping if both objs are solid.
				BGE_2DVect overlap;
				if ( boxCollision(others[i], &overlap)) {
					interact(others[i], overlap);
				}
			}
		}
	}
}

void BGE_Object::say(std::string message) {
    if (messageTexture != NULL) {
        delete messageTexture;
    }
    messageTexture = new BGE_Texture;
    messageTexture->loadFromRenderedTextOnFrame(message, 0,0,0);
    messageTimer = 2;
}

void BGE_Object::die() {
#ifdef DEBUG
    if (type == CREATURE) {
        printf("%s died.\n", getName().c_str());
    }
    else {
        printf("%s destroyed.\n", getName().c_str());
    }
#endif // DEBUG
    engine->remove(this);
}

void BGE_Object::render() {
    if (messageTexture != NULL) {
        messageTexture->render(position.x, position.y-getDepth());
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

float BGE_Object::getHealthPercent() {
    return health/getMaxHealth()*100;
}

BGE_Object::Use BGE_Object::getUse() {
	return dataOf[type].use;
}

float BGE_Object::getVolume() {
	return dataOf[type].depth * dataOf[type].height * dataOf[type].width;
}

float BGE_Object::getDepth() {
    return dataOf[static_cast<int>(type)].depth;
}

float BGE_Object::getMass() {
    return getVolume() * dataOfMaterial[static_cast<int>(material)].density;
}

float BGE_Object::getNutrition() {
	return getMass() * dataOfMaterial[static_cast<int>(material)].nutrition;
}

float BGE_Object::getMaxHealth() {
    return dataOf[static_cast<int>(type)].baseHealth*dataOfMaterial[static_cast<int>(material)].strenght;
}

BGE_Object::PhysicalState BGE_Object::getPhysicalState() {
	return dataOfMaterial[static_cast<int>(material)].state;
}

float BGE_Object::getReloadTime() {
    return dataOf[static_cast<int>(type)].reloadTime;   //SIGSEGV segmentation fault. this == NULL
}

std::string BGE_Object::getName() {
    return dataOf[type].name + " of " + dataOfMaterial[static_cast<int>(material)].name;
}
