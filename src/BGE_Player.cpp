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

BGE_Player::BGE_Player() {
	//Initialise clip to rest
	activeItem = NULL;
	status = IDLE;
	animationCtr = 0;
	healthPoints = 5;
	//angle = -0.523;

	//Set object collision properties.
	colliderWidth = 25;
	colliderHeight = 25;
	setCollision(true);

	//Set object properties.
	BGE_ObjectMaterial material;
	material.material = BGE_ObjectMaterial::Material::FLESH;
	material.state = BGE_ObjectMaterial::PhysicalState::SOFT_SOLID;
	type.type = BGE_ObjectType::Type::CREATURE;
    type.material = material;
}

BGE_Player::~BGE_Player() {}

void BGE_Player::setSoundFxs( Mix_Chunk *wound, Mix_Chunk *die ) {
    BGE_Player::woundFx = wound;
    BGE_Player::dieFx = die;
}

void BGE_Player::handleEvent( SDL_Event &e ) {
	//If mouse is clicked
	if ( e.type == SDL_MOUSEBUTTONDOWN ) {
		printf("HP: %f\n", healthPoints);
		if (activeItem != NULL) {
            use(activeItem);
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
			std::vector<BGE_Object *>::iterator index = std::find(content.begin(), content.end(), activeItem);
			index += e.wheel.y;
            while (index < content.begin()) {
				index += content.size();
            }
            while (index >= content.end()) {
				index -= content.size();
            }
            activeItem = (*index);
            printf("Active Item: %s\n", activeItem->type.getName().c_str());
		}
	}
	//If a key was pressed
	else if( e.type == SDL_KEYDOWN && e.key.repeat == 0 && status != DIE ) {
		//Adjust the speed
		switch( e.key.keysym.sym ) {
			case SDLK_UP:
				speed.y -= SPEED;
				break;

			case SDLK_DOWN:
				speed.y += SPEED;
				break;

			case SDLK_LEFT:
				speed.x -= SPEED;
				status = WALKING;
				//flip = SDL_FLIP_HORIZONTAL;
				break;

			case SDLK_RIGHT:
				speed.x += SPEED;
				status = WALKING;
				//flip = SDL_FLIP_NONE;
				break;

			case SDLK_s:
				status = ATTACK;
				animationCtr = 0;
				break;

			case SDLK_k:
				status = DIE;
				animationCtr = 0;
				break;

			case SDLK_q:
				// TODO set angle to private in base class and use getters/setters (want to keep it within 0-2pi).
				angle += 0.1;
				break;

			case SDLK_w:
				angle -= 0.1;
				break;
		}
	}
	//If a key was released
	else if( e.type == SDL_KEYUP && e.key.repeat == 0 && status != DIE ) {
		//Adjust the velocity
		switch( e.key.keysym.sym ) {
			case SDLK_UP:
				speed.y += SPEED;
				break;

			case SDLK_DOWN:
				speed.y -= SPEED;
				break;

			case SDLK_LEFT:
				speed.x += SPEED;
				status = IDLE;
				break;

			case SDLK_RIGHT:
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
						animationCtr = 0;
						break;

					case 1: //circle
						break;

					case 2: //X
						status = ATTACK;
						animationCtr = 0;
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
    if (attackAnimationCtr <= 0) {
		texture->renderSprite(position.x, position.y, 0, 1, SDL_FLIP_NONE,  angle*57.29577951);
	}
	else {
		attackAnimationCtr--;
	}
}

void BGE_Player::walk() {
	if ( status != DIE ) {
			if ( status == IDLE ) {
					if ( flip == SDL_FLIP_NONE ) {
							speed.x = SPEED / 2;
						}
					else {
							speed.x = -SPEED / 2;
						}

					status = WALKING;
				}

			if( position.x < 20 ) {
					flip = SDL_FLIP_NONE;
					speed.x = SPEED / 2;
				}
			else if( position.x > BGE_Engine::SCREEN_WIDTH - colliderWidth - 20 ) {
					flip = SDL_FLIP_HORIZONTAL;
					speed.x = -SPEED / 2;
				}
		}
}

void BGE_Player::applySpeed(float Dt) {
	animationCtr++;

	switch ( status ) {
		case IDLE:
			if ( animationCtr >= 2 * FRAME_REPEAT ) {
				animationCtr = 0;
			}

			break;

		case WALKING:
			if ( animationCtr >= 4 * FRAME_REPEAT ) {
				animationCtr = 0;
			}

			break;

		case ATTACK:
			if ( animationCtr >= 3 * FRAME_REPEAT ) {
				animationCtr = 0;
				status = IDLE;
			}

			break;

		case DIE:
			if ( animationCtr >= 3 * FRAME_REPEAT ) {
				animationCtr = 3 * FRAME_REPEAT - 1;
			}

			break;

		case WOUNDED:
			if ( animationCtr >= 3 * FRAME_REPEAT ) {
				animationCtr = 0;
				status = IDLE;
			}

			break;
	}

	BGE_Object::applySpeed( Dt );
	updateAngle();
}

void BGE_Player::interact(BGE_Object *other, BGE_2DVect overlap) {
    if (other->type.getVolume() <= 1.0) {
		//Object small enough to be picked up.
        add(other);
        printf("%s added to inventory!\n", other->type.getName().c_str());
    }
    else if (other->type.getMass() <= 0.5) {
		//Object light enough to be moved.
		BGE_2DVect otherPosition = other->getPosition();
		other->setPosition(otherPosition.x - overlap.x, otherPosition.y - overlap.y);
    }
    else {
		//Object is an obstacle.
		position = position + overlap;
    }
#ifdef DEBUG
    printf("Interacted with a %s", other->type.getName().c_str());
    printf(" mass:%f\n", other->type.getMass());
    printf("Active Item: %s\n", activeItem->type.getName().c_str());
#endif // DEBUG
}

void BGE_Player::use( BGE_Object *object) {
	switch (object->type.getUse()) {
		case BGE_ObjectType::Use::WEAPON: {
			//Throw it.
			BGE_2DVect initialPosition (position.x, position.y);
			//I'm using initialSpeed as an offset...
			BGE_2DVect initialSpeed;
			initialSpeed.setPolar( getCollisionRadius() + object->getCollisionRadius()+2, angle);
			initialPosition = initialPosition + initialSpeed;
			initialSpeed = initialSpeed * 3.0;
			object->setPosition(initialPosition.x, initialPosition.y);
			object->setSpeed(initialSpeed.x, initialSpeed.y);
			remove(object);
			break;
		}
		case BGE_ObjectType::Use::HANDHELD_WEAPON:
			//attackAnimationCtr = 5;
			break;
		case BGE_ObjectType::Use::SHOOTING_WEAPON:
			printf("Shooting weapon\n");
			switch (object->type.type) {
				case BGE_ObjectType::Type::GUN:
					printf("Gun\n");
					//Check if there are bullets.
                    for (int i=0; i<content.size(); i++) {
                        if (content[i]->type.type == BGE_ObjectType::Type::BULLETS) {
							printf("Bullets\n");
							std::vector<BGE_Object *> others = engine->getOthers();
							//Remove this from others.
							std::vector<BGE_Object *>::iterator thisIndex;
							thisIndex = std::find(others.begin(), others.end(), this);
							others.erase(thisIndex);
							//Check for collision with a dummy object.
                            BGE_Object dummyBullet;
                            BGE_2DVect startPosition;
                            for (int j=0; j<100; j++) {//Range of bullet.
								bool collisionFound = false;
								startPosition.setPolar(getCollisionRadius()+2.0*j, angle);
								startPosition = startPosition + position;
								dummyBullet.setPosition(startPosition.x, startPosition.y);
								for (int l=0; l<others.size(); l++) {
									if (others[l]->circularCollision(&dummyBullet)) {
										if (others[l]->boxCollision(&dummyBullet)) {
											//Collision found!
                                            others[l]->wound(1);
                                            startPosition.setPolar(50, angle);
                                            others[l]->setSpeed(startPosition.x, startPosition.y);
                                            printf("Collision found!");
                                            collisionFound = true;
                                            break;
										}
									}
								}
								if (collisionFound) {
									break;
								}
							}
                            return;
                        }
                    }
                    printf("No bullets available!\n");
					break;
				default:
					break;
			}
			break;
		case BGE_ObjectType::Use::FOOD: {
            healthPoints = healthPoints + object->type.getToxicity();
            printf("Current HP: %f\n", healthPoints);
            printf("total toxicity HP: %f\n", object->type.getToxicity());
            remove(object);
            //TODO should also remove from world.
			break;
		}
		case BGE_ObjectType::Use::FURNITURE: {
			//Place it.
			BGE_2DVect initialPosition;
			initialPosition.setPolar( getCollisionRadius() + object->getCollisionRadius()+2, angle);
			initialPosition = initialPosition + position;
			object->setPosition(initialPosition.x, initialPosition.y);
			remove(object);
			break;
		}
		case BGE_ObjectType::Use::TRANSPORTATION:
			break;
		default:
			break;
	}
}

void BGE_Player::wound( int damage ) {
	healthPoints -= damage;

	if ( healthPoints <= 0 ) {
			status = DIE;
			animationCtr = 0;
			speed.x = 0;
			Mix_PlayChannel( -1, dieFx, 0 );
		}
	else {
			status = WOUNDED;
			animationCtr = 0;
			speed.x = 0;
			Mix_PlayChannel( -1, woundFx, 0 );
		}
}

void BGE_Player::setSpeed(float x, float y) {
	BGE_Object::setSpeed(x, y );
	if ( x == 0 && y == 0 ) {
		status = IDLE;
	}
	else {
		status = WALKING;
	}
}

void BGE_Player::updateAngle() {
	BGE_2DVect correction (0,12.5);
	BGE_2DVect mouseRel = mousePositionOnScreen - (position - engine->getViewportOffset()) + correction;
	setAngle(mouseRel.angle());
}

void BGE_Player::add( BGE_Object *object) {
	BGE_Object::add(object);
	if (activeItem == NULL) {
		activeItem = object;
	}
}

void BGE_Player::remove( BGE_Object *object) {
	BGE_Object::remove( object);
	if (activeItem == object) {
		if (content.empty()) {
			activeItem = NULL;
		}
		else {
			activeItem = content[0];
		}
	}
}

