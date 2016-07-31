// Commented includes are reduntant to Game.h

// Definitions for this class
#include "BGE_Engine.h"

// Definitions for other classes of the project
//#include "BGE_Texture.h"
#include <BGE_Object.h>
#include <BGE_Timer.h>
#include <BGE_2DRect.h>

// External includes
#include <SDL2/SDL.h>       //SDL library
#include <SDL2/SDL_image.h> //used to load png images (and other formats)
#include <SDL2/SDL_ttf.h>   //used to render text to images/textures
#include <SDL2/SDL_mixer.h> //used to play sounds
#include <stdio.h>
//#include <vector>
#include <fstream>
#include <chrono>
#include <algorithm>	//used for std::find and std::sort

// Contains global preprocessor defines.
#include <flags.h>

const float BGE_Engine::FREE_MOVE_ZONE = 0.5;

BGE_Engine::BGE_Engine() : loadedObjects( 6 ), visibleObjects( 6 ) {
	//Initialise random number generator.
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	randomGen.seed(seed);

	//Initialise pointers.
	window = NULL;
	renderer = NULL;
	joystick = NULL;
	BGE_Object::engine = this;
	BGE_ObjectType::engine = this;

	dot.setPosition( 50, 50);
	it1.setPosition( 100, 20 );
	it1.type.setRandom();
	it2.setPosition( 500, 100 );
	it2.type.setRandom();
	it3.setPosition( 70, 150 );
	it3.type.setRandom();
	it4.setPosition( 500, 300 );
	it4.type.type = BGE_ObjectType::Type::GUN;
	it5.setPosition( 10, 420 );
	it5.type.type = BGE_ObjectType::Type::BULLETS;
}

BGE_Engine::~BGE_Engine() {}

bool BGE_Engine::init() {
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK ) < 0 ) {
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else {
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) ) {
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		//Check for joysticks
		if( SDL_NumJoysticks() < 1 ) {
			printf( "No joysticks found, using keyboard.\n" );
		}
		else {
			//Load joystick
			joystick = SDL_JoystickOpen( 0 );

			if( joystick == NULL ) {
				printf( "Warning: Unable to open game controller! SDL Error: %s\nUsing keyboard.\n", SDL_GetError() );
			}
		}

		//Create window
		window = SDL_CreateWindow( "Angry man", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );

		if( window == NULL ) {
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else {
			//Create vsynced renderer for window
			renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );

			if( renderer == NULL ) {
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else {
				//Initialize renderer color
				SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;

				if( !( IMG_Init( imgFlags ) & imgFlags ) ) {
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}

				//Initialize SDL_ttf
				if( TTF_Init() == -1 ) {
					printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
					success = false;
				}

				//Initialise Audio mixer
				if ( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 ) {
					printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
					success = false;
				}
			}
		}
	}

	return success;
}

bool BGE_Engine::load() {
	//Loading success flag
	bool success = true;

	//Load textures
	BGE_Texture::engine = this;
	if( !stickmanSheet.loadFromFile( "img/stickman.png" ) ) {
		printf( "Failed to load stickman texture!\n" );
		success = false;
	}
	else {
		stickmanSheet.setSpriteSize( 50, 50);
		stickmanSheet.setSpriteOffset(0, -12);
	}
	if( !itemSheet.loadFromFile( "img/itemsheet.png" ) ) {
		printf( "Failed to load items texture!\n" );
		success = false;
	}
	else {
		itemSheet.setSpriteSize( 25, 50);
		itemSheet.setSpriteOffset(0, -12);
	}

	//Load text
	std::ifstream inputListFile ( "list" );
	std::string lines [100];
	int ctr = 0;

	while ( std::getline( inputListFile, lines[ctr] ) ) {
		ctr++;
	}

	//Load fonts:
	defaultFont = TTF_OpenFont( "fnt/FreeMonoBold.ttf", 14 );

	if( defaultFont == NULL ) {
		printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
		success = false;
	}
	else {
		if ( !textTest.loadFromRenderedText( lines[getRandom(0, ctr-1)], 0, 0, 0 ) ) {
			printf( "Failed to load texture from text.\n" );
			success = false;
		}
	}

	//Load sound fx
	ouchFx = Mix_LoadWAV( "snd/ouch.wav" );

	if ( ouchFx == NULL ) {
		printf( "Failed to load ouch sfx\n" );
		success = false;
	}

	muoioFx = Mix_LoadWAV( "snd/muoio.wav" );

	if ( ouchFx == NULL ) {
		printf( "Failed to load ouch sfx\n" );
		success = false;
	}

	dot.setSoundFxs( ouchFx, muoioFx );

	//Assign textures
	dot.setTexture( &stickmanSheet );
	it1.setTexture( &itemSheet );
	it2.setTexture( &itemSheet );
	it3.setTexture( &itemSheet );
	it4.setTexture( &itemSheet );
	it5.setTexture( &itemSheet );

	return success;
}

void BGE_Engine::start() {
	//Main loop flag
	bool quit = false;

	//Event handler
	SDL_Event e;

	//FPS timer, used to correct for fluctuations
	BGE_Timer FPStimer;
	FPStimer.start();

	//Create list of objects
	loadedObjects[0] = &dot;
	loadedObjects[1] = &it1;
	loadedObjects[2] = &it2;
	loadedObjects[3] = &it3;
	loadedObjects[4] = &it4;
	loadedObjects[5] = &it5;
	//Create list of visible objects
	visibleObjects[0] = &dot;
	visibleObjects[1] = &it1;
	visibleObjects[2] = &it2;
	visibleObjects[3] = &it3;
	visibleObjects[4] = &it4;
	visibleObjects[5] = &it5;

	//Select controller
	void ( BGE_Object::*eventHandler )( SDL_Event & );

	if ( joystick == NULL ) {
		eventHandler = &BGE_Object::handleEvent;
	}
	else {
		eventHandler = &BGE_Object::handleEventJoy;
	}

	//While application is running
	while( !quit ) {
		//Handle events on queue
		while( SDL_PollEvent( &e ) != 0 ) {
			//User requests quit
			if( e.type == SDL_QUIT ) {
				quit = true;
			}

			//Handle input for the player
			( dot.*eventHandler )( e );
		}

		//Time between frames in S
		float frameTime = FPStimer.getMs() / 1000.0f;
		//Restart the timer
		FPStimer.start();

		//Apply speed to objects
		// TODO should be replaced by a more significative update() function.
		for ( int i = 0; i < loadedObjects.size(); i++ ) {
			loadedObjects[i]->applySpeed( frameTime );
		}
		//Apply collisions, for each couple of objects applyCollision is called once.
		//Note player should be always first.
		for ( std::vector<BGE_Object*>::iterator i = loadedObjects.begin(); i < loadedObjects.end(); i++) {
            std::vector<BGE_Object*> remainingObjects (i+1, loadedObjects.end());
            (*i)->applyCollision( remainingObjects );
		}

		//Adjust viewport.
		BGE_2DRect freeMove = getFreeMoveArea();
		BGE_2DRect player = loadedObjects[0]->getCollisionBox();
        if (!freeMove.contains(player)) {
			if (player.x < freeMove.x) {
                viewportOffset.x -= freeMove.x-player.x;
			}
			else if (player.x+player.w > freeMove.x+freeMove.w) {
				viewportOffset.x += player.x+player.w-freeMove.x-freeMove.w;
			}
			if (player.y < freeMove.y) {
                viewportOffset.y -= freeMove.y-player.y;
			}
			else if (player.y+player.h > freeMove.y+freeMove.h) {
				viewportOffset.y += player.y+player.h-freeMove.y-freeMove.h;
			}
        }

        //Update visible object list.
        visibleObjects.clear();
        BGE_2DRect viewport = getViewport();
        for ( int i = 0; i < loadedObjects.size(); i++ ) {
			BGE_2DRect object = loadedObjects[i]->getCollisionBox();
            if (viewport.overlaps(object)) {
				//Object is inside the viewport.
                visibleObjects.push_back(loadedObjects[i]);
            }
        }

		//Clear screen
		SDL_SetRenderDrawColor( renderer, 100, 110, 100, 0xFF );
		SDL_RenderClear( renderer );

		//Render visible objects
		std::sort(visibleObjects.begin(), visibleObjects.end(), compareRenderLevel);
		for ( int i = 0; i < visibleObjects.size(); i++ ) {
			if (visibleObjects[i]->isVisible()) {
				visibleObjects[i]->render();
			}
		}

#ifdef DEBUG
		//Render collision boxes
		SDL_SetRenderDrawColor( renderer, 0,0,255, 255);
		for ( int i = 0; i < loadedObjects.size(); i++ ) {
			BGE_2DRect box = loadedObjects[i]->getCollisionBox();
			box -= viewportOffset;
			SDL_Point points [5];
            points[0] = {box.x,box.y};
            points[1] = {box.x+box.w,box.y};
            points[2] = {box.x+box.w,box.y+box.h};
            points[3] = {box.x,box.y+box.h};
            points[4] = {box.x,box.y};
			SDL_RenderDrawLines(renderer, points, 5);
		}
#endif // DEBUG

		//Render test text
		textTest.render( SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 );

		//Update screen
		SDL_RenderPresent( renderer );
	}
}

void BGE_Engine::close() {
	//Free loaded images
	stickmanSheet.free();
	itemSheet.free();
	textTest.free();

	//Free fonts
	TTF_CloseFont( defaultFont );
	defaultFont = NULL;

	//Free loaded sound fx
	Mix_FreeChunk( ouchFx );
	Mix_FreeChunk( muoioFx );
	ouchFx = NULL;
	muoioFx = NULL;

	//Unload joystick
	SDL_JoystickClose( joystick );
	joystick = NULL;

	//Destroy window
	SDL_DestroyRenderer( renderer );
	SDL_DestroyWindow( window );
	window = NULL;
	renderer = NULL;

	//Quit SDL subsystems
	Mix_Quit();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

void BGE_Engine::show(BGE_Object* object) {
	if (object != NULL ) {
		//if object is loaded: (not really necessary though...)
		if (std::find(loadedObjects.begin(), loadedObjects.end(), object) != loadedObjects.end()) {
            visibleObjects.push_back(object);
		}
		else {
			printf("Error: show(): no such object.\n");
		}
	}
}

void BGE_Engine::hide(BGE_Object *object) {
	if (object != NULL ) {
		//if object is visible:
		std::vector<BGE_Object *>::iterator objectIndex;
		objectIndex = std::find(visibleObjects.begin(), visibleObjects.end(), object);
		if ( objectIndex != visibleObjects.end() ) {
            visibleObjects.erase( objectIndex);
		}
		else {
			printf("Error: hide(): object was not visible.\n");
		}
	}
}

SDL_Renderer *BGE_Engine::getRenderer() {
	return renderer;
}

TTF_Font *BGE_Engine::getFont() {
	return defaultFont;
}

std::vector<BGE_Object *> BGE_Engine::getOthers() {
	return loadedObjects;
}

int BGE_Engine::getRandom(int min, int max) {
	std::uniform_int_distribution<int> uniformDist (min, max);
	return uniformDist( randomGen);
}

BGE_2DRect BGE_Engine::getFreeMoveArea() {
	BGE_2DRect freeMove (viewportOffset.x+SCREEN_WIDTH*FREE_MOVE_ZONE/2,viewportOffset.y+SCREEN_HEIGHT*FREE_MOVE_ZONE/2,SCREEN_WIDTH*FREE_MOVE_ZONE,SCREEN_HEIGHT*FREE_MOVE_ZONE);
	return freeMove;
}

BGE_2DRect BGE_Engine::getViewport() {
	BGE_2DRect viewport (viewportOffset.x, viewportOffset.y, SCREEN_WIDTH, SCREEN_HEIGHT);
	return viewport;
}

BGE_2DVect BGE_Engine::getViewportOffset() {
	return viewportOffset;
}

bool BGE_Engine::compareRenderLevel(BGE_Object *front, BGE_Object *back) {
	return front->getPosition().y < back->getPosition().y;
}
