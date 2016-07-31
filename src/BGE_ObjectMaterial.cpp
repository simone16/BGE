#include "BGE_ObjectMaterial.h"

BGE_ObjectMaterial::BGE_ObjectMaterial() {}

BGE_ObjectMaterial::~BGE_ObjectMaterial() {}

BGE_ObjectMaterial::Material BGE_ObjectMaterial::getMaterial() {
    return material;
}

BGE_ObjectMaterial::PhysicalState BGE_ObjectMaterial::getState() {
    return state;
}

float BGE_ObjectMaterial::getDensity() {
    float density = 1;
    switch (material) {
        case Material::FLESH:
            density = 1;
            break;
		case Material::BONE:
            density = 0.5;
            break;
		case Material::IRON:
            density = 50;
            break;
		case Material::PINEWOOD:
            density = 0.5;
            break;
		case Material::MARBLE:
            density = 10;
            break;
		case Material::FIBERGLASS:
            density = 0.2;
            break;
		case Material::DRUG:
            density = 0.5;
            break;
		case Material::WATER:
            density = 1;
            break;
		case Material::FABRIC:
            density = 0.1;
            break;
		default:
            printf("No defined density for material: %i\n", material);
            break;
    }
    return density;
}

float BGE_ObjectMaterial::getToxicity() {
    //Negative value => good for you!
    switch (material) {
        case Material::FLESH:
            return -1.0;
        case Material::BONE:
            return 0.5;
        case Material::IRON:
            return 1.0;
        case Material::PINEWOOD:
            return 0.2;
        case Material::MARBLE:
            return 1.0;
        case Material::FIBERGLASS:
            return 1.5;
        case Material::DRUG:
            return -0.2;
        case Material::WATER:
            return -0.5;
        case Material::FABRIC:
            return 0.0;
        default:
            return 0.0;
    }
}

std::string BGE_ObjectMaterial::getName() {
    return getStateName() + " " + getMaterialName();
}

std::string BGE_ObjectMaterial::getMaterialName() {
    switch (material) {
        case Material::FLESH:
            return "flesh";
		case Material::BONE:
            return "bone";
		case Material::IRON:
            return "iron";
		case Material::PINEWOOD:
            return "pine wood";
		case Material::MARBLE:
            return "marble";
		case Material::FIBERGLASS:
            return "fiberglass";
		case Material::DRUG:
            return "drugs";
		case Material::WATER:
            return "water";
		case Material::FABRIC:
            return "fabric";
		default:
            return "unknown";
    }
}

std::string BGE_ObjectMaterial::getStateName() {
    switch (state) {
        case PhysicalState::HARD_SOLID:
            return "solid";
        case PhysicalState::SOFT_SOLID:
            return "soft";
        case PhysicalState::LIQUID:
            return "liquid";
        case PhysicalState::GAS:
            return "gaseous";
        case PhysicalState::PLASMA:
            return "plasma";
        default:
            return "unknown";
    }
}
