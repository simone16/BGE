#include "BGE_Enemy.h"

#include <BGE_Engine.h>
#include <BGE_Item.h>
#include <BGE_Player.h>

#include <algorithm>

const float BGE_Enemy::CP_TOLERANCE = 10;

BGE_Enemy::BGE_Enemy( CreatureType crtType ):
    BGE_Creature( crtType ) {
    useTriggered = false;
    usePreDelay = 0;
}

BGE_Enemy::~BGE_Enemy() {}

void BGE_Enemy::update( float Dt) {
    //If a decision has already been made, no need to think...
    if (usePreDelay > 0) {
        usePreDelay -= Dt;
    }
    else {
        //it's time to think about this...
        if (useTriggered) {
            use();
            useTriggered = false;
        }
        //First: look for blackListed objects
        if (!blackList.empty()) {
            //For each blacklisted object
            for (std::vector<BGE_Object*>::iterator badGuy = blackList.begin();
                    badGuy < blackList.end(); badGuy++) {
                //If badGuy is visible
                if (canSee(*badGuy)) {
                    BGE_2DVect badGuyRelPos = (*badGuy)->position -position;
                    //Behaviour depends on activeItem
                    switch (activeItem->getUse()) {
                        case BGE_Object::Use::FOOD : {
                            //If activeItem is FOOD, change it
                            std::vector<BGE_Item*>::iterator item = content.begin();
                            while (item != content.end()) {
                                if ((*item)->getUse() != BGE_Object::Use::FOOD) {
                                    activeItem = (*item);
                                    break;
                                }
                                item++;
                            }
                            if (item == content.end()) {
                                //Do nothing if this only has food
                                break;
                            }
                        }
                        case BGE_Object::Use::HANDHELD_WEAPON :
                            //if activeItem is handheld, engage close proximity combat
                            speed.setPolar(getCreatureData().runSpeed, badGuyRelPos.angle());
                            break;
                        default :
                            //shoot/throw from the distance
                            speed.x = 0;
                            speed.y = 0;
                    }
                    target = (*badGuy)->position;
                    usePreDelay = getCreatureData().responseTime;
                    useTriggered = true;
                    //No need to check for other blaklisted objs
                    break;
                }
            }
        }
        //Second: patrol checkpoints
        else if (!checkPoints.empty()) {
            BGE_2DVect CheckPntRelPos = *currentCheckPoint - position;
            BGE_Object *firstCollision;
            BGE_2DVect collPnt;
            //if path to checkpoint is free
            if (!segmentCollision( position, *currentCheckPoint, &firstCollision, &collPnt)) {
                //if checkpoint reached
                if (CheckPntRelPos.modulus() <= CP_TOLERANCE) {
                    //activate next checkpoint
                    currentCheckPoint++;
                    if (currentCheckPoint == checkPoints.end()) {
                        currentCheckPoint = checkPoints.begin();
                    }
                }
                //Go towards checkpoint
                speed.setPolar(getCreatureData().walkSpeed, CheckPntRelPos.angle());
            }
            else {
                //Path to checkpoint is not free.
            }
        }
        //Third: move to a random place
        else {
            if ((position - target).modulus() <= CP_TOLERANCE) {
                //Target reached.
                target.setPolar(engine->getNormalRandom(200, 100), engine->getRandomFloat(0,TWO_PI));
                speed.setPolar(getCreatureData().walkSpeed, target.angle());
                target += position;
            }
            else if (speed.x == 0 && speed.y == 0) {
                speed.setPolar(getCreatureData().walkSpeed, (target-position).angle());
            }
        }
    }
    BGE_Creature::update(Dt);
}

void BGE_Enemy::interact(BGE_Object* other, BGE_2DVect overlap) {
    BGE_Creature::interact(other, overlap);
    BGE_2DVect newTarget;
    newTarget.setPolar(engine->getNormalRandom(200, 100), engine->getRandomFloat(0,TWO_PI));
    speed.setPolar(getCreatureData().walkSpeed, newTarget.angle());
    target = position + newTarget;
}

void BGE_Enemy::hit(BGE_Object* origin, float energy) {
    BGE_Creature::hit(origin, energy);
    notifyHit(origin, this);
}

void BGE_Enemy::notifyHit(BGE_Object* attacker, BGE_Creature* victim) {
    //If this is a cop
    if (creatureType == CreatureType::COP) {
        //Add attacker to blacklist
        std::vector<BGE_Object*>::iterator index = std::find( blackList.begin(), blackList.end(), attacker);
        if (index == blackList.end()) {
            blackList.push_back(attacker);
        }
    }
    else {
        //Run, you fool!
        target.setPolar(engine->getNormalRandom(300, 100), (position - attacker->position).angle());
        speed.setPolar(getCreatureData().runSpeed, target.angle());
        target += position;
    }
}

void BGE_Enemy::addCheckPoint(BGE_2DVect checkPoint) {
    checkPoints.push_back(checkPoint);
    currentCheckPoint = checkPoints.begin();    //Necessary in case of reallocation.
}
