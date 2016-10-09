#include "BGE_Item.h"

#include <BGE_Texture.h>
#include <BGE_Engine.h>
#include <BGE_Particle.h>

#include <cmath>
#include <stdio.h>
#include <algorithm>	//used for std::find

#include <flags.h>

BGE_Item::BGE_Item( Type objectType, Material objMaterial):
    BGE_Moveable(objectType, objMaterial) {
    selfDestruct = false;
}

BGE_Item::~BGE_Item() {}

void BGE_Item::update(float Dt) {
    if (selfDestruct) {
        //Use health as countdown.
        health -= Dt*getData().selfDestructTimeConv;
    }
    //Am I dead?
    BGE_Object::update( Dt);
    //My speed!
    BGE_Moveable::update( Dt);
}

void BGE_Item::interact(BGE_Object* other, BGE_2DVect overlap) {
    //If other is a tile...
    if (other->type == BGE_Object::TILE) {
        BGE_Moveable::wallBounce(other, overlap);
    }
    //Other is not a tile (then we assume it is Moveable child, god help us).
    else {
        BGE_Moveable *otherMov = static_cast<BGE_Moveable*>(other);
        BGE_Moveable::elasticBounce(otherMov, overlap);
    }
}

void BGE_Item::hit(BGE_Object* origin, float energy) {
    BGE_Object::hit(origin, energy);
    BGE_2DVect effect = position - origin->position;
    effect.setPolar( std::sqrt(2*energy/getMass()), effect.angle());
    speed += effect;
}

void BGE_Item::die() {
    engine->playItem();
    for (int i=0; i<content.size(); i++) {
        content[i]->position = position;
        content[i]->speed.setPolar(engine->getNormalRandom(100, 50), engine->getRandomFloat(0, TWO_PI));
        content[i]->setAsContent(false);
    }
    content.clear();
    if ( type == GRENADE) {
        float damage = getData().baseDamage;
        BGE_Particle::explosion(position, damage, Material::IRON, 0.0, TWO_PI);
        std::vector<BGE_Object *> targets = engine->getMoveableObjects();
        for (int i=0; i<targets.size(); i++) {
            float distance = (targets[i]->position - position).modulus();
            //don't do anything if the resulting damage is < 100;
            if (targets[i] != this && targets[i]->canCollide() && distance < damage/100 && isFirstCollision(targets[i])) {
                targets[i]->hit(this, damage/(distance+0.5));
            }
        }
    }
    BGE_Object::die();
}

void BGE_Item::setAsContent(bool content) {
    visible = !content;
    collides = !content;
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
    SDL_RendererFlip flip;
    if (angle > TWO_PI/4 && angle < TWO_PI*3/4) {
        flip = SDL_FLIP_VERTICAL;
    }
    else {
        flip = SDL_FLIP_NONE;
    }
#ifdef INCOMPLETE_SHEETS
    if (dataOf[type].sheet == NULL) {
        engine->itemSheetSmall.renderSprite(position.x, position.y, 0, 0, flip,  angle*DEGREE_OVER_RADIANS);
        return;
    }
#endif // INCOMPLETE_SHEETS
    dataOf[type].sheet->renderSprite(position.x, position.y, dataOf[type].column, 0, flip,  angle*DEGREE_OVER_RADIANS);
    BGE_Object::render();
}

float BGE_Item::getDamage() {
    if (type == BULLETS) {
        return getData().baseDamage * getMass();
    }
    else if (getData().use == Use::HANDHELD_WEAPON) {
        return getData().baseDamage * getMaterialData().strenght;
    }
    else {
#ifdef DEBUG
        printf("Warning, use of undefined damage for %s!\n", getName().c_str());
#endif // DEBUG
        return 0;
    }
}
