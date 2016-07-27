#include "BGE_Stickman.h"

#include "BGE_Engine.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <cmath>

Mix_Chunk *BGE_Stickman::woundFx = NULL;
Mix_Chunk *BGE_Stickman::dieFx = NULL;

BGE_Stickman::BGE_Stickman() {
	//Initialise clip to rest
	status = IDLE;
	animationCtr = 0;
	healthPoints = 5;
	//angle = -0.523;

	//Set object size
	colliderWidth = 25;
	colliderHeight = 50;
	colliderRadius = (std::sqrt( std::pow(colliderWidth, 2) + std::pow(colliderHeight, 2) )/2 + CIRCULAR_COLLISION_PADDING);
}

BGE_Stickman::~BGE_Stickman() {}

void BGE_Stickman::setSoundFxs( Mix_Chunk *wound, Mix_Chunk *die ) {
    BGE_Stickman::woundFx = wound;
    BGE_Stickman::dieFx = die;
}

void BGE_Stickman::handleEvent( SDL_Event &e ) {
	//If a key was pressed
	if( e.type == SDL_KEYDOWN && e.key.repeat == 0 && status != DIE ) {
			//Adjust the velocity
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
						flip = SDL_FLIP_HORIZONTAL;
						break;

					case SDLK_RIGHT:
						speed.x += SPEED;
						status = WALKING;
						flip = SDL_FLIP_NONE;
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
	else if ( e.type == SDL_JOYBUTTONDOWN ) {
			printf( "Joystick pressed: %i\n", e.jbutton.button );
		}
	else if ( e.type == SDL_JOYAXISMOTION ) {
			printf( "Joystick moved axis %i at position: %i\n", e.jaxis.axis, e.jaxis.value );
		}
	else if ( e.type == SDL_JOYHATMOTION ) {
			printf( "Joystick moved hat %i at position: %i\n", e.jhat.hat, e.jhat.value );
		}
}

void BGE_Stickman::handleEventJoy( SDL_Event &e ) {
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

void BGE_Stickman::render() {
    texture->renderSprite(position.x, position.y, animationCtr/FRAME_REPEAT, status, flip, angle*57.29577951);
}

void BGE_Stickman::walk() {
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

void BGE_Stickman::wound( int damage ) {
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

void BGE_Stickman::move(float Dt, std::vector<BGE_Object *> others) {
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

	if (speed.x != 0 || speed.y != 0) {
		BGE_Object::move(Dt, others);

		for ( int i = 0; i < others.size(); i++ ) {
			if ( this != others[i] ) {
				if (circularCollision(others[i]) /*&& status == ATTACK && animationCtr == 1 * FRAME_REPEAT*/) {
					BGE_2DVect collision, correction;
					if (boxCollision(others[i], &collision, &correction)) {
						//others[i]->wound(1);
						position = position + correction;
					}
				}
			}
		}
	}
}

void BGE_Stickman::setSpeed(float x, float y) {
	BGE_Object::setSpeed(x, y );
	if ( x == 0 && y == 0 ) {
		status = IDLE;
	}
	else {
		status = WALKING;
	}
}

