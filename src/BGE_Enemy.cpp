#include "BGE_Enemy.h"

#include <BGE_Engine.h>
#include <BGE_Item.h>
#include <BGE_Player.h>

BGE_Enemy::BGE_Enemy( CreatureType crtType ):
    BGE_Creature( crtType ) {
}

BGE_Enemy::~BGE_Enemy() {}

void BGE_Enemy::update( float Dt) {
    switch (creatureType) {
        case CreatureType::COP: {
            BGE_Player *player = engine->getPlayer();
            bool attack = false;
            if ((position - player->position).modulus() <= 400) {
                //Sees the player.
                BGE_2DVect playerRel;
                playerRel = player->position -position;
                speed.setPolar(SPEED/2, playerRel.angle());
                target = player->position;
                attack = true;
            }
            else if ((position - target).modulus() <= 10) {
                //Target reached.
                BGE_2DVect newTarget;
                newTarget.setPolar(engine->getNormalRandom(200, 100), engine->getRandomFloat(0,TWO_PI));
                speed.setPolar(SPEED/5, newTarget.angle());
                target = position + newTarget;
            }
            BGE_Creature::update(Dt);
            if (attack) {
                use();
            }
            break;
        }
        case CreatureType::COWBOY: {
            if (!checkPoints.empty()) {
                BGE_2DVect relativeCP = *currentCheckPoint - position;
                if (relativeCP.modulus() <= 10) {
                    //Checkpoint reached.
                    currentCheckPoint++;
                    if (currentCheckPoint == checkPoints.end()) {
                        currentCheckPoint = checkPoints.begin();
                    }
                }
                //Go towards checkpoint.
                speed.setPolar(SPEED/5, relativeCP.angle());
            }
            else {
            }
            BGE_Creature::update(Dt);
        }
    }
}

void BGE_Enemy::interact(BGE_Object* other, BGE_2DVect overlap) {
    BGE_Creature::interact(other, overlap);
    BGE_2DVect newTarget;
    newTarget.setPolar(engine->getNormalRandom(200, 100), engine->getRandomFloat(0,TWO_PI));
    speed.setPolar(SPEED/5, newTarget.angle());
    target = position + newTarget;
}

void BGE_Enemy::addCheckPoint(BGE_2DVect checkPoint) {
    checkPoints.push_back(checkPoint);
    currentCheckPoint = checkPoints.begin();    //Necessary in case of reallocation.
}
