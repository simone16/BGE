#include "BGE_ObjectType.h"

#include <BGE_Engine.h>

BGE_Engine *BGE_ObjectType::engine = NULL;

BGE_ObjectType::TypeData BGE_ObjectType::dataOf[TOT];

BGE_ObjectType::BGE_ObjectType( Type _typeCode ) {
    typeCode = _typeCode;
}

BGE_ObjectType::~BGE_ObjectType() {}

void BGE_ObjectType::init() {
    //                  Name        Use          Z_Depth, Width, Height
    dataOf[BARREL] =    {"Barrel",Use::FURNITURE     ,30,25,25};
    dataOf[BLOCK] =     {"Block",Use::FURNITURE     ,25,25,25};
    dataOf[BOTTLE] =    {"Bottle",Use::WEAPON        ,20,10,10};
    dataOf[BULLETS] =   {"Bullets",Use::NONE          ,10,10,10};
    dataOf[CAR] =       {"Car",Use::TRANSPORTATION,50,100,50};
    dataOf[CHAIR] =     {"Chair",Use::FURNITURE     ,50,25,25};
    dataOf[GUN] =       {"Gun",Use::SHOOTING_WEAPON,20,15,10};
    dataOf[CREATURE] =  {"Creature",Use::FOOD          ,50,25,25};
    dataOf[MOUSE] =     {"Mouse",Use::FOOD          ,10,10,10};
    dataOf[KNIFE] =     {"Knife",Use::HANDHELD_WEAPON,10,10,10};
    dataOf[STONE] =     {"Stone",Use::WEAPON        ,10,10,10};
    dataOf[SWORD] =     {"Sword",Use::HANDHELD_WEAPON,10,25,10};
    dataOf[TABLE] =     {"Table",Use::FURNITURE     ,25,25,25};
    dataOf[TILE] =      {"Tile",Use::NONE           ,25,25,25};
}

void BGE_ObjectType::setRandom() {
    typeCode = static_cast<Type> (engine->getRandom(0, static_cast<int> (Type::TOT) -1));
//    material.material = static_cast<BGE_ObjectMaterial::Material> (engine->getRandom(0, static_cast<int> (BGE_ObjectMaterial::Material::TOT)-1));
//    material.state = static_cast<BGE_ObjectMaterial::PhysicalState> (engine->getRandom(0, static_cast<int> (BGE_ObjectMaterial::PhysicalState::TOT)-1));
}


std::string BGE_ObjectType::getName() {
    return dataOf[typeCode].name;
}

BGE_ObjectType::Use BGE_ObjectType::getUse() {
    return dataOf[typeCode].use;
}

float BGE_ObjectType::getVolume() {
    TypeData data = dataOf[typeCode];
    return data.depth*data.height*data.width;
}

BGE_2DRect BGE_ObjectType::getBox() {
    TypeData data = dataOf[typeCode];
    BGE_2DRect box (-data.width/2 ,-data.height/2 ,data.width, data.height);
    return box;
}

bool BGE_ObjectType::canMove() {
    return getUse() != Use::FURNITURE;
}
