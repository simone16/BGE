#include "BGE_Player.h"

#include "BGE_Engine.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <cmath>
#include <algorithm>

#include <stdio.h>

#include <flags.h>

Mix_Chunk *BGE_Player::woundFx = NULL;
Mix_Chunk *BGE_Player::dieFx = NULL;

const float BGE_Player::POCKETS_VOLUME = 10000;
const float BGE_Player::MOVE_WEIGHT_THRESHOLD = 100;
const float BGE_Player::THROW_ENERGY = 4000;

BGE_Player::BGE_Player() {
	activeItem = NULL;
	status = IDLE;

	//Set object properties.
	material = Material::FLESH;
	type = CREATURE;
}

BGE_Player::~BGE_Player() {}

void BGE_Player::setSoundFxs( Mix_Chunk *wound, Mix_Chunk *die ) {
    BGE_Player::woundFx = wound;
    BGE_Player::dieFx = die;
}

void BGE_Player::handleEvent( SDL_Event &e ) {
	//If mouse is clicked
	if ( e.type == SDL_MOUSEBUTTONDOWN ) {
		if ( e.button.button == SDL_BUTTON_LEFT) {
			if (activeItem != NULL) {
				use();
			}
		}
		else if (e.button.button == SDL_BUTTON_RIGHT) {
			if (activeItem != NULL) {
				dispose();
			}
		}
		else if (e.button.button == SDL_BUTTON_MIDDLE) {
			printf("HP: %f\n", health);

		}
	}
	//If mouse is moved
	else if ( e.type == SDL_MOUSEMOTION) {
		//Update mouse position.
		int x,y;
        SDL_GetMouseState(&x,&y);
        mousePositionOnScreen.x = x;
        mousePositionOnScreen.y = y;
        //Update angle.
        updateAngle();
	}
	//If mouse wheel is rotated
	else if (e.type == SDL_MOUSEWHEEL) {
		if (!content.empty()) {
			std::vector<BGE_Item *>::iterator index = std::find(content.begin(), content.end(), activeItem);
			index += e.wheel.y;
            while (index < content.begin()) {
				index += content.size();
            }
            while (index >= content.end()) {
				index -= content.size();
            }
            activeItem = (*index);
            printf("Active Item: %s\n", activeItem->getName().c_str());
		}
	}
	//If a key was pressed
	else if( e.type == SDL_KEYDOWN && e.key.repeat == 0 && status != DIE ) {
		//Adjust the speed
		switch( e.key.keysym.sym ) {
			case SDLK_w:
				speed.y -= SPEED;
				break;

			case SDLK_s:
				speed.y += SPEED;
				break;

			case SDLK_a:
				speed.x -= SPEED;
				status = WALKING;
				//flip = SDL_FLIP_HORIZONTAL;
				break;

			case SDLK_d:
				speed.x += SPEED;
				status = WALKING;
				//flip = SDL_FLIP_NONE;
				break;

			case SDLK_l:
				status = ATTACK;
				break;

			case SDLK_k:
				status = DIE;
				break;

			case SDLK_q:
				// TODO set angle to private in base class and use getters/setters (want to keep it within 0-2pi).
				angle += 0.1;
				break;

			case SDLK_e:
				angle -= 0.1;
				break;
		}
	}
	//If a key was released
	else if( e.type == SDL_KEYUP && e.key.repeat == 0 && status != DIE ) {
		//Adjust the velocity
		switch( e.key.keysym.sym ) {
			case SDLK_w:
				speed.y += SPEED;
				break;

			case SDLK_s:
				speed.y -= SPEED;
				break;

			case SDLK_a:
				speed.x += SPEED;
				status = IDLE;
				break;

			case SDLK_d:
				speed.x -= SPEED;
				status = IDLE;
				break;
		}
	}
}

void BGE_Player::handleEventJoy( SDL_Event &e ) {
	if ( e.type == SDL_JOYHATMOTION ) {
			switch ( e.jhat.value ) {
					case SDL_HAT_CENTERED:
						speed.x = 0;
						speed.y = 0;
						status = IDLE;
						break;

					case SDL_HAT_DOWN:
						speed.x = 0;
						speed.y = SPEED;
						status = WALKING;
						break;

					case SDL_HAT_LEFT:
						speed.x = -SPEED;
						speed.y = 0;
						status = WALKING;
						flip = SDL_FLIP_HORIZONTAL;
						break;

					case SDL_HAT_LEFTDOWN:
						speed.x = -SPEED;
						speed.y = SPEED;
						status = WALKING;
						flip = SDL_FLIP_HORIZONTAL;
						break;

					case SDL_HAT_LEFTUP:
						speed.x = -SPEED;
						speed.y = -SPEED;
						status = WALKING;
						flip = SDL_FLIP_HORIZONTAL;
						break;

					case SDL_HAT_RIGHT:
						speed.x = SPEED;
						speed.y = 0;
						status = WALKING;
						flip = SDL_FLIP_NONE;
						break;

					case SDL_HAT_RIGHTDOWN:
						speed.x = SPEED;
						speed.y = SPEED;
						status = WALKING;
						flip = SDL_FLIP_NONE;
						break;

					case SDL_HAT_RIGHTUP:
						speed.x = SPEED;
						speed.y = -SPEED;
						status = WALKING;
						flip = SDL_FLIP_NONE;
						break;

					case SDL_HAT_UP:
						speed.x = 0;
						speed.y = -SPEED;
						status = WALKING;
						break;
				}
		}
	else if ( e.type == SDL_JOYBUTTONDOWN ) {
			switch ( e.jbutton.button ) {
					case 0: //triangle
						status = DIE;
						break;

					case 1: //circle
						break;

					case 2: //X
						status = ATTACK;
						break;

					case 3: //square
						break;
				}
		}
}

void BGE_Player::render() {
    int quadrant;
    if (angle < 0.7853) {
		quadrant = 2;
    }
    else if (angle < 2.3562) {
		quadrant = 0;
    }
    else if (angle < 3.9270) {
		quadrant = 3;
    }
    else if (angle < 5.4978) {
		quadrant = 1;
    }
    else {
		quadrant = 2;
    }

    texture->renderSprite(position.x, position.y, quadrant, 0, flip);
    if (activeItem == NULL) { //bula bula scherzone
		texture->renderSprite(position.x, position.y, 0, 1, SDL_FLIP_NONE,  angle*DEGREE_OVER_RADIANS);
	}
	else {
		activeItem->render();
	}
}

void BGE_Player::update(float Dt) {
	//animationCtr++;

//	switch ( status ) {
//		case IDLE:
//			if ( animationCtr >= 2 * FRAME_REPEAT ) {
//				animationCtr = 0;
//			}
//
//			break;
//
//		case WALKING:
//			if ( animationCtr >= 4 * FRAME_REPEAT ) {
//				animationCtr = 0;
//			}
//
//			break;
//
//		case ATTACK:
//			if ( animationCtr >= 3 * FRAME_REPEAT ) {
//				animationCtr = 0;
//				status = IDLE;
//			}
//
//			break;
//
//		case DIE:
//			if ( animationCtr >= 3 * FRAME_REPEAT ) {
//				animationCtr = 3 * FRAME_REPEAT - 1;
//			}
//
//			break;
//
//		case WOUNDED:
//			if ( animationCtr >= 3 * FRAME_REPEAT ) {
//				animationCtr = 0;
//				status = IDLE;
//			}
//
//			break;
//	}

	position += speed*Dt;
	updateAngle();
}

void BGE_Player::interact(BGE_Object *other, BGE_2DVect overlap) {
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
#ifdef DEBUG
    printf("Interacted with a %s", other->getName().c_str());
    printf(" mass:%f\n", other->getMass());
    if (activeItem != NULL) {
		printf("Active Item: %s\n", activeItem->getName().c_str());
    }
#endif // DEBUG
}

void BGE_Player::use() {
	switch (activeItem->getUse()) {
		case BGE_Object::Use::WEAPON: {
			//Throw it.
			//Place object outside collision range.
            activeItem->position.setPolar(getCollisionRadius() + activeItem->getCollisionRadius()+2, angle);
            activeItem->position += position;
            //Set object speed according to throw strenght.
            activeItem->speed.setPolar( std::sqrt(2*THROW_ENERGY/activeItem->getMass()), angle);
			remove(activeItem);
			break;
		}
		case BGE_Object::Use::HANDHELD_WEAPON:
			break;
		case BGE_Object::Use::SHOOTING_WEAPON:
			printf("Shooting weapon\n");
			switch (activeItem->type) {
				case BGE_Object::GUN:
					printf("Gun\n");
					//Check if has any bullets.
                    for (int i=0; i<content.size(); i++) {
                        if (content[i]->type == BGE_Object::BULLETS) {
							printf("Bullets\n");
							std::vector<BGE_Object *> others = engine->getCollidingObjects();
							std::vector<BGE_Object *> collided;
							std::vector<BGE_2DVect> collisionPoints;
                            BGE_2DVect bulletEnd;
                            bulletEnd.setPolar(3000, angle);
                            bulletEnd += position;
                            for (int j=0; j<others.size(); j++) {
                                BGE_2DVect a,b;
                                if (others[j] != this && others[j]->canCollide() && others[j]->getCollisionBox().intersection(position, bulletEnd, a, b)) {
									collided.push_back(others[j]);
									collisionPoints.push_back(a);
									collisionPoints.push_back(b);
                                }
                            }
                            if (!collisionPoints.empty()) {
								BGE_2DVect *collision = &collisionPoints[0];
								BGE_Object *hit = collided[0];
								for (int j=1; j<collisionPoints.size(); j++) {
									if ((collisionPoints[j]-position).modulus() < (*collision - position).modulus()) {
										collision = &collisionPoints[j];
										hit = collided[j/2];
									}
								}
                                hit->hit(position, 2000);
                                BGE_Object *splinters = new BGE_Object;
								splinters->texture = &(engine->splintersSheet);
								splinters->position = *collision;
								splinters->type = SPLINTERS;
								splinters->material = hit->material;
								engine->add(splinters);
							}
							else {
								printf("Miss\n");
							}
							BGE_Object *splinters = new BGE_Object;
							splinters->texture = &(engine->splintersSheet);
							splinters->position.setPolar(30, angle);
							splinters->position += position;
							splinters->type = SPLINTERS;
							splinters->material = Material::IRON;
							engine->add(splinters);
                            return;
                        }
                    }
                    printf("No bullets available!\n");
					break;
				default:
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
		case BGE_Object::Use::TRANSPORTATION:
			break;
		default:
			break;
	}
}

void BGE_Player::dispose() {
	activeItem->position.setPolar( getCollisionRadius() + activeItem->getCollisionRadius()+2, angle);
	activeItem->position += position;
	static_cast<BGE_Item *>(activeItem)->setAngle(0);
	remove(activeItem);
}

void BGE_Player::updateAngle() {
	BGE_2DVect correction (0,12.5);
	BGE_2DVect mouseRel = mousePositionOnScreen - (position - engine->getViewportOffset()) + correction;
	BGE_Object::setAngle(mouseRel.angle());
	if (activeItem != NULL) {
		activeItem->position.setPolar(25, angle);
		activeItem->position += position;
		static_cast<BGE_Item*>(activeItem)->setAngle(angle);
	}
}

void BGE_Player::add( BGE_Item *item) {
	BGE_Item::add(item);
	if (activeItem == NULL) {
		activeItem = item;
	}
}

void BGE_Player::remove( BGE_Item *item) {
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

