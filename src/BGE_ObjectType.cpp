#include "BGE_ObjectType.h"

#include <BGE_Engine.h>

BGE_Engine *BGE_ObjectType::engine = NULL;

BGE_ObjectType::BGE_ObjectType() {}

BGE_ObjectType::~BGE_ObjectType() {}

void BGE_ObjectType::setRandom() {
    type = static_cast<Type> (engine->getRandom(0, static_cast<int> (Type::TOT) -1));
    material.material = static_cast<BGE_ObjectMaterial::Material> (engine->getRandom(0, static_cast<int> (BGE_ObjectMaterial::Material::TOT)-1));
    material.state = static_cast<BGE_ObjectMaterial::PhysicalState> (engine->getRandom(0, static_cast<int> (BGE_ObjectMaterial::PhysicalState::TOT)-1));
}

BGE_ObjectType::Type BGE_ObjectType::getType() {
    return type;
}

BGE_ObjectType::Use BGE_ObjectType::getUse() {
    switch (type) {
        case Type::BARREL:
            return Use::FURNITURE;
            break;
        case Type::BLOCK:
            return Use::FURNITURE;
            break;
        case Type::BOTTLE:
            return Use::WEAPON;
            break;
        case Type::BULLETS:
            return Use::NONE;
            break;
        case Type::CAR:
            return Use::TRANSPORTATION;
            break;
        case Type::CHAIR:
            return Use::FURNITURE;
            break;
        case Type::GUN:
            return Use::SHOOTING_WEAPON;
            break;
        case Type::CREATURE:
            return Use::FOOD;
            break;
        case Type::MOUSE:
            return Use::FOOD;
            break;
        case Type::KNIFE:
            return Use::HANDHELD_WEAPON;
            break;
        case Type::STONE:
            return Use::WEAPON;
            break;
        case Type::SWORD:
            return Use::HANDHELD_WEAPON;
            break;
        case Type::TABLE:
            return Use::FURNITURE;
            break;
        default:
            return Use::FURNITURE;
            printf("No defined use for this item!!\n");
            break;
    }
}

BGE_ObjectMaterial BGE_ObjectType::getMaterial() {
    return material;
}

float BGE_ObjectType::getVolume() {
    float volume = 0;
    switch (type) {
        case Type::BARREL:
            volume = 1;
            break;
        case Type::BLOCK:
            volume = 1;
            break;
        case Type::BOTTLE:
            volume = 0.5;
            break;
        case Type::BULLETS:
            volume = 0.1;
            break;
        case Type::CAR:
            volume = 10;
            break;
        case Type::CHAIR:
            volume = 0.75;
            break;
        case Type::GUN:
            volume = 0.5;
            break;
        case Type::CREATURE:
            volume = 1;
            break;
        case Type::MOUSE:
            volume = 0.2;
            break;
        case Type::KNIFE:
            volume = 0.2;
            break;
        case Type::STONE:
            volume = 0.2;
            break;
        case Type::SWORD:
            volume = 0.5;
            break;
        case Type::TABLE:
            volume = 0.75;
            break;
        default:
            printf("No defined volume for item: %i\n", type);
            break;
    }
    return volume;
}

float BGE_ObjectType::getMass() {
    return getVolume() * material.getDensity();
}

float BGE_ObjectType::getToxicity() {
    return getMass() * material.getToxicity();
}

bool BGE_ObjectType::canMove() {
    return getUse() != Use::FURNITURE;
}

std::string BGE_ObjectType::getTypeName() {
    switch (type) {
        case Type::BARREL:
            return "Barrel";
            break;
        case Type::BLOCK:
            return "Block";
            break;
        case Type::BOTTLE:
            return "Bottle";
            break;
        case Type::BULLETS:
            return "Bullets";
            break;
        case Type::CAR:
            return "Car";
            break;
        case Type::CHAIR:
            return "Chair";
            break;
        case Type::GUN:
            return "Gun";
            break;
        case Type::CREATURE:
            return "Creature";
            break;
        case Type::MOUSE:
            return "Mouse";
            break;
        case Type::KNIFE:
            return "Knife";
            break;
        case Type::STONE:
            return "Stone";
            break;
        case Type::SWORD:
            return "Sword";
            break;
        case Type::TABLE:
            return "Table";
            break;
        default:
            return "Unknown";
            break;
    }
}

std::string BGE_ObjectType::getName() {
    return getTypeName() + " of " + material.getName();
}
