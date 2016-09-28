#include "BGE_Creature.h"

#include "BGE_Engine.h"
#include <BGE_Enemy.h>
#include <BGE_Particle.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <cmath>
#include <algorithm>

#include <flags.h>

const float BGE_Creature::POCKETS_VOLUME = 10000;
const float BGE_Creature::MOVE_WEIGHT_THRESHOLD = 100;
const float BGE_Creature::THROW_ENERGY = 4000;

BGE_Creature::BGE_Creature( CreatureType crtType):
	BGE_Item( CREATURE, Material::FLESH) {
	creatureType = crtType;
	//Item constructor calls items getMaxHealth, so...
	health = getMaxHealth();
	//No active item at first.
	activeItem = NULL;
	//Initialise animation counters.
	weaponAnimCtr = 0;
	//Ready to shoot.
	useDelay = 0;
}

BGE_Creature::~BGE_Creature() {}

void BGE_Creature::render() {
	BGE_Object::render();
    uint8_t bodyQuadrant = (target-position).quadrant();
    //Determine whether creature is moving horizontally.
	bool horizontal = false;
    if (bodyQuadrant == 0 || bodyQuadrant == 2) {
		horizontal = true;
    }

	//Flip hor. when facing left.
	if (bodyQuadrant == 2) {
		flip = SDL_FLIP_HORIZONTAL;
	}
	else {
		flip = SDL_FLIP_NONE;
	}

	//Use clip 0 if not moving, else advance ctr.
	Uint8 column;
	if (speed.modulus() == 0) {
		column = 0;
		animCtr = 0;
	}
	else {
		animCtr++;
		if (animCtr > WALK_FRAMES*WALK_FRAME_REPEAT) {
			animCtr = WALK_FRAME_REPEAT;
		}
		//If moving backwards invert animation.
		uint8_t speedQuadrant = speed.quadrant();
		if (std::abs(speedQuadrant-bodyQuadrant) == 2) {
			column = WALK_FRAMES - animCtr/WALK_FRAME_REPEAT;
		}
		else {
			column = animCtr/WALK_FRAME_REPEAT;
		}
	}

	//Render body.
    engine->stickmanSheet.renderSprite(position.x, position.y, column, horizontal?1:0, flip);

    if (activeItem == NULL) { //bula bula scherzone
		//texture->renderSprite(position.x, position.y, 0, 1, SDL_FLIP_NONE,  angle*DEGREE_OVER_RADIANS);
	}
	else {
		if (weaponAnimCtr > 0) {
			switch (activeItem->getUse()) {
				case BGE_Object::Use::HANDHELD_WEAPON: {
					//Push & retract.
					BGE_2DVect offset;
					offset.setPolar(weaponAnimCtr*0.5, angle);
					activeItem->position += offset;
					weaponAnimCtr--;
					break;
				}
				case BGE_Object::Use::SHOOTING_WEAPON:
					//Recoil (rotation).
					float offset = -weaponAnimCtr/DEGREE_OVER_RADIANS*3;
					if (angle > TWO_PI/4 && angle < TWO_PI*3/4) {
						offset = -offset;
					}
                    activeItem->setAngle(angle + offset);
                    weaponAnimCtr--;
					break;
			}
		}
		//Render activeItem.
		activeItem->render();
	}

	//Determine which hat sprite to use.
	if (horizontal) {
		column = 0;
	}
	else if (bodyQuadrant == 1) {
		column = 1;
	}
	else  {
		column = 2;
	}
	//Render hat.
	engine->hatsSheet.renderSprite(position.x, position.y, column, dataOfCreature[static_cast<int>(creatureType)].spriteIndex, flip);
}

void BGE_Creature::update(float Dt) {
	BGE_Object::update(Dt);
	//Update position.
	position += speed*Dt;
	updateAngle();
	//Update delays.
	if (useDelay > 0) {
		useDelay -= Dt;
	}
}

void BGE_Creature::interact(BGE_Object *other, BGE_2DVect overlap) {
	if (other->type == BGE_Object::TILE) {
		//Other is a tile.
		position += overlap;
	}
    else if (other->getVolume() <= POCKETS_VOLUME) {
		//Put in inventory.
        add(static_cast<BGE_Item*>(other));
        printf("%s added to inventory!\n", other->getName().c_str());
    }
    else if (other->getMass() <= MOVE_WEIGHT_THRESHOLD) {
		//Drag around.
		other->position -= overlap;
    }
    else {
		//Can't move other.
		position += overlap;
    }
    std::string message;
    message = "OMG! A ";
	message += other->getName();
    say(message);
}

void BGE_Creature::hit(BGE_Object *origin, float energy) {
	//Affect health
	BGE_Object::hit( origin, energy);
	//Scream in pain
    std::vector<BGE_Enemy *> creatures = engine->getAICreatures();
    for (std::vector<BGE_Enemy *>::iterator creature = creatures.begin();
			creature < creatures.end(); creature++) {
        //If creature is seeing this dying (what an orrible sight!)
        if ((*creature)->canSee(this)) {
			(*creature)->notifyHit(origin, this);
        }
	}
}

void BGE_Creature::use() {
	if (activeItem != NULL && useDelay <= 0) {
		useDelay = activeItem->getReloadTime();	//In some cases activeItem is NULL at the end of use().
		//Avoid surgical precision when using items
		float useAngle = angle + engine->getNormalRandom(0,getData().aimAccuracy);
		switch (activeItem->getUse()) {
			case BGE_Object::Use::WEAPON: {
				//Throw it.
				//Place object outside collision range.
				activeItem->position.setPolar(getCollisionRadius() + activeItem->getCollisionRadius()+2, useAngle);
				activeItem->position += position;
				//Set object speed according to throw strenght.
				activeItem->speed.setPolar( std::sqrt(2*THROW_ENERGY/activeItem->getMass()), useAngle);
				remove(activeItem);
				break;
			}
			case BGE_Object::Use::HANDHELD_WEAPON: {
				BGE_2DVect range;
				range.setPolar(activeItem->getCollisionRadius()*2+getCollisionRadius(), useAngle);
				range += position;
				BGE_2DVect collisionPoint;
				BGE_Object *collisionObj = NULL;
				if (segmentCollision(position, range, &collisionObj, &collisionPoint)) {
					collisionObj->hit(this, 2000);
					BGE_Particle *splinters = new BGE_Particle(collisionObj->material );
					splinters->position = collisionPoint;
					engine->add(splinters);
				}
				weaponAnimCtr = 5*WALK_FRAME_REPEAT;
				break;
			}
			case BGE_Object::Use::SHOOTING_WEAPON:
				switch (activeItem->type) {
					case BGE_Object::GUN:
						//Check if has any bullets.
						for (int i=0; i<content.size(); i++) {
							if (content[i]->type == BGE_Object::BULLETS) {
								BGE_2DVect bulletEnd;
								bulletEnd.setPolar(3000, useAngle);
								bulletEnd += position;
								BGE_2DVect collisionPoint;
								BGE_Object *collisionObj = NULL;
								if (segmentCollision(position, bulletEnd, &collisionObj, &collisionPoint)) {
									collisionObj->hit(this, 2000);
									BGE_Particle *splinters = new BGE_Particle(collisionObj->material);
									splinters->position = collisionPoint;
									engine->add(splinters);
								}
								BGE_Particle *splinters = new BGE_Particle(Material::IRON);
								splinters->position.setPolar(30, angle);
								splinters->position += position;
								splinters->speed.setPolar(300, angle);
								engine->add(splinters);
								weaponAnimCtr = 5*WALK_FRAME_REPEAT;
								break;
							}
						}
						break;
				}
				break;
			case BGE_Object::Use::FOOD: {
				health += activeItem->getNutrition();
				printf("Current HP: %f\n", health);
				printf("total toxicity HP: %f\n", activeItem->getNutrition());
				engine->remove(activeItem);
				remove(activeItem);
				break;
			}
		}
	}
}

void BGE_Creature::dispose() {
	if (activeItem != NULL) {
		activeItem->position.setPolar( getCollisionRadius() + activeItem->getCollisionRadius()+2, angle);
		activeItem->position += position;
		static_cast<BGE_Item *>(activeItem)->setAngle(0);
		remove(activeItem);
	}
}

void BGE_Creature::updateAngle() {
	BGE_2DVect mouseRel = target - position;
	BGE_Object::setAngle(mouseRel.angle());
	if (activeItem != NULL) {
		activeItem->position.setPolar(25, angle);
		activeItem->position += position;
		activeItem->setAngle(angle);
	}
}

void BGE_Creature::add( BGE_Item *item) {
	BGE_Item::add(item);
	if (activeItem == NULL) {
		activeItem = item;
	}
}

void BGE_Creature::remove( BGE_Item *item) {
	BGE_Item::remove( item);
	if (activeItem == item) {
		if (content.empty()) {
			activeItem = NULL;
		}
		else {
			activeItem = content[0];
		}
	}
}

bool BGE_Creature::canSee(BGE_Object* target) {
	//Check target is within view field
	if ((position - target->position).modulus() <= getViewField()) {
		BGE_Object *firstCollision;
		BGE_2DVect collPnt;
		segmentCollision(position, target->position, &firstCollision, &collPnt);
		//If the first obstruction is the target, vision is possible.
		return firstCollision == target;
	}
	else {
		return false;
	}
}

float BGE_Creature::getUseDelayPercent() {
	if (activeItem != NULL) {
		float percent = useDelay/activeItem->getReloadTime()*100;
		if (percent < 0) {
			return 0;
		}
		else {
			return percent;
		}
	}
	else {
		return 0;
	}
}

float BGE_Creature::getMaxHealth() {
	return dataOfCreature[static_cast<int>(creatureType)].health;
}

float BGE_Creature::getViewField() {
	return getData().viewField;
}

std::string BGE_Creature::getName() {
    return dataOfCreature[static_cast<int>(creatureType)].name;
}

bool BGE_Creature::segmentCollision(BGE_2DVect start, BGE_2DVect end, BGE_Object** firstCollision, BGE_2DVect* collision) {
	std::vector<BGE_Object *> others = engine->getCollidingObjects();
	//List all objects which intersect the segment, adding two collision points for each.
	std::vector<BGE_Object *> collided;
	std::vector<BGE_2DVect> collisionPoints;
	for (int j=0; j<others.size(); j++) {
		BGE_2DVect a,b;
		if (others[j] != this && others[j]->canCollide() && others[j]->getCollisionBox().intersection(start, end, a, b)) {
			collided.push_back(others[j]);
			collisionPoints.push_back(a);
			collisionPoints.push_back(b);
		}
	}
	//Get the nearest colliding object and the nearest point.
	if (!collisionPoints.empty()) {
		*collision = collisionPoints[0];
		*firstCollision = collided[0];
		for (int j=1; j<collisionPoints.size(); j++) {
			if ((collisionPoints[j]-start).modulus() < (*collision - start).modulus()) {
				*collision = collisionPoints[j];
				*firstCollision = collided[j/2];
			}
		}
		return true;
	}
	else {
		return false;
	}
}

BGE_Object::CreatureData BGE_Creature::getData() {
    return dataOfCreature[static_cast<int>(creatureType)];
}
