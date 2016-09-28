#include "BGE_Item.h"

#include <BGE_Texture.h>
#include "BGE_Engine.h"

#include <cmath>
#include <stdio.h>
#include <algorithm>	//used for std::find

#include <flags.h>

BGE_Item::BGE_Item( Type objectType, Material objMaterial):
    BGE_Moveable(objectType, objMaterial) {
}

BGE_Item::~BGE_Item() {}

void BGE_Item::update(float Dt) {
    BGE_Moveable::update( Dt);
}

void BGE_Item::interact(BGE_Object* other, BGE_2DVect overlap) {
    //If other is a tile...
    if (other->type == BGE_Object::TILE) {
        BGE_Moveable::wallBounce(other, overlap);
    }
    //Other is not a tile (then we assume it is Moveable child, god help us).
    else {
        BGE_Moveable::elasticBounce(other, overlap);
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
