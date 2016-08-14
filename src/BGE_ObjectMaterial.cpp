#include "BGE_ObjectMaterial.h"

//Assumes 25px -> 0.5m
const float BGE_ObjectMaterial::LITERS_PER_CUBIC_PX = 1/(8*156.25);

BGE_ObjectMaterial::MaterialData BGE_ObjectMaterial::dataOf[TOT];

BGE_ObjectMaterial::BGE_ObjectMaterial( Material _materialCode) {
    materialCode = _materialCode;
}

BGE_ObjectMaterial::~BGE_ObjectMaterial() {}

void BGE_ObjectMaterial::init() {
    //                   name       state                     density nutrition
    dataOf[FLESH] =     {"flesh",   PhysicalState::SOFT_SOLID   ,1.0*LITERS_PER_CUBIC_PX,1.0};
    dataOf[BONE] =      {"bone",    PhysicalState::HARD_SOLID   ,0.7*LITERS_PER_CUBIC_PX,0.1};
    dataOf[IRON] =      {"iron",    PhysicalState::HARD_SOLID   ,7.9*LITERS_PER_CUBIC_PX,-0.5};
    dataOf[PINEWOOD] =  {"pine wood",PhysicalState::HARD_SOLID  ,0.5*LITERS_PER_CUBIC_PX,-0.1};
    dataOf[MARBLE] =    {"marble",  PhysicalState::HARD_SOLID   ,2.7*LITERS_PER_CUBIC_PX,-1.0};
    dataOf[FIBERGLASS] = {"fiberglass",PhysicalState::HARD_SOLID,2.0*LITERS_PER_CUBIC_PX,-2.0};
    dataOf[DRUG] =      {"drugs",   PhysicalState::SOFT_SOLID   ,1.0*LITERS_PER_CUBIC_PX,0.0};
    dataOf[WATER] =     {"water",   PhysicalState::LIQUID       ,1.0*LITERS_PER_CUBIC_PX,0.5};
    dataOf[FABRIC] =    {"fabric",  PhysicalState::SOFT_SOLID   ,0.2*LITERS_PER_CUBIC_PX,0.1};
}

std::string BGE_ObjectMaterial::getName() {
    return dataOf[materialCode].name;
}

BGE_ObjectMaterial::PhysicalState BGE_ObjectMaterial::getState() {
    return dataOf[materialCode].state;
}

float BGE_ObjectMaterial::getDensity() {
    return dataOf[materialCode].density;
}

float BGE_ObjectMaterial::getNutrition() {
    return dataOf[materialCode].nutrition;
}


