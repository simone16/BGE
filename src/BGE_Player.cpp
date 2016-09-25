#include "BGE_Player.h"

#include <BGE_Engine.h>

#include <algorithm>
#include <cstdio>

BGE_Player::BGE_Player():
	BGE_Creature( CreatureType::PLAYER ){
}

BGE_Player::~BGE_Player() {}

void BGE_Player::handleEvent( SDL_Event &e ) {
	//If mouse is clicked
	if ( e.type == SDL_MOUSEBUTTONDOWN ) {
		if ( e.button.button == SDL_BUTTON_LEFT) {
			use();
		}
		else if (e.button.button == SDL_BUTTON_RIGHT) {
			dispose();
		}
		else if (e.button.button == SDL_BUTTON_MIDDLE) {
			printf("HP: %f\n", health);
			printf("position: %i, %i\n", int(position.x), int(position.y));

		}
	}
	//If mouse is moved
	else if ( e.type == SDL_MOUSEMOTION) {
		//Update target position.
		int x,y;
        SDL_GetMouseState(&x,&y);
        target.x = x;
        target.y = y + 12.5;	//Correction term.
        target += engine->getViewportOffset();
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
	else if( e.type == SDL_KEYDOWN && e.key.repeat == 0) {
		//Adjust the speed
		switch( e.key.keysym.sym ) {
			case SDLK_w:
				speed.y -= getData().walkSpeed;
				break;

			case SDLK_s:
				speed.y += getData().walkSpeed;
				break;

			case SDLK_a:
				speed.x -= getData().walkSpeed;
				break;

			case SDLK_d:
				speed.x += getData().walkSpeed;
				break;
		}
	}
	//If a key was released
	else if( e.type == SDL_KEYUP && e.key.repeat == 0) {
		//Adjust the velocity
		switch( e.key.keysym.sym ) {
			case SDLK_w:
				speed.y += getData().walkSpeed;
				break;

			case SDLK_s:
				speed.y -= getData().walkSpeed;
				break;

			case SDLK_a:
				speed.x += getData().walkSpeed;
				break;

			case SDLK_d:
				speed.x -= getData().walkSpeed;
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
						break;

					case SDL_HAT_DOWN:
						speed.x = 0;
						speed.y = getData().walkSpeed;
						break;

					case SDL_HAT_LEFT:
						speed.x = -getData().walkSpeed;
						speed.y = 0;
						break;

					case SDL_HAT_LEFTDOWN:
						speed.x = -getData().walkSpeed;
						speed.y = getData().walkSpeed;
						break;

					case SDL_HAT_LEFTUP:
						speed.x = -getData().walkSpeed;
						speed.y = -getData().walkSpeed;
						break;

					case SDL_HAT_RIGHT:
						speed.x = getData().walkSpeed;
						speed.y = 0;
						break;

					case SDL_HAT_RIGHTDOWN:
						speed.x = getData().walkSpeed;
						speed.y = getData().walkSpeed;
						break;

					case SDL_HAT_RIGHTUP:
						speed.x = getData().walkSpeed;
						speed.y = -getData().walkSpeed;
						break;

					case SDL_HAT_UP:
						speed.x = 0;
						speed.y = -getData().walkSpeed;
						break;
				}
		}
	else if ( e.type == SDL_JOYBUTTONDOWN ) {
			switch ( e.jbutton.button ) {
					case 0: //triangle
						break;

					case 1: //circle
						break;

					case 2: //X
						break;

					case 3: //square
						break;
				}
		}
}

void BGE_Player::die() {
	printf("Player died!\n");
    engine->gameOver();
}
