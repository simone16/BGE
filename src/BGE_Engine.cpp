// Commented includes are reduntant to Game.h

// Definitions for this class
#include "BGE_Engine.h"

// Definitions for other classes of the project
//#include "BGE_Texture.h"
#include "BGE_Object.h"
#include "BGE_Timer.h"

// External includes
#include <SDL2/SDL.h>       //SDL library
#include <SDL2/SDL_image.h> //used to load png images (and other formats)
#include <SDL2/SDL_ttf.h>   //used to render text to images/textures
#include <SDL2/SDL_mixer.h> //used to play sounds
#include <stdio.h>
//#include <vector>
#include <fstream>
#include <chrono>
#include <random>

// Contains global preprocessor defines.
#include <flags.h>

BGE_Engine::BGE_Engine() : objects( 6 ) {
	window = NULL;
	renderer = NULL;
	joystick = NULL;
	BGE_Object::engine = this;

	enemy.setPosition( 50, 20 );
	enemy1.setPosition( 500, 100 );
	enemy1.setSpeed( -BGE_Object::SPEED / 2, 0 );
	enemy2.setPosition( 70, 150 );
	enemy3.setPosition( 500, 300 );
	enemy3.setSpeed( -BGE_Object::SPEED / 2, 0 );
	enemy4.setPosition( 10, 420 );
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
	}

	if( !stickmanUmbrellaSheet.loadFromFile( "img/stickmanUmbrella.png" ) ) {
		printf( "Failed to load stickman with umbrella texture!\n" );
		success = false;
	}
	else {
		stickmanUmbrellaSheet.setSpriteSize( 50, 50);
	}

	//Load text
	std::ifstream inputListFile ( "list" );
	std::string lines [100];
	int ctr = 0;

	while ( std::getline( inputListFile, lines[ctr] ) ) {
		ctr++;
	}

	//Create a random distribution
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine randomGen( seed );
	std::uniform_int_distribution<int> uniformDist( 0, ctr - 1 );
	// get a random value with uniformDist(randomGen) : int;

	//Load fonts:
	defaultFont = TTF_OpenFont( "fnt/FreeMonoBold.ttf", 14 );

	if( defaultFont == NULL ) {
		printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
		success = false;
	}
	else {
		if ( !textTest.loadFromRenderedText( lines[uniformDist( randomGen )], 0, 0, 0 ) ) {
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
	enemy.setSoundFxs( ouchFx, muoioFx );
	enemy1.setSoundFxs( ouchFx, muoioFx );
	enemy2.setSoundFxs( ouchFx, muoioFx );
	enemy3.setSoundFxs( ouchFx, muoioFx );
	enemy4.setSoundFxs( ouchFx, muoioFx );

	//Assign textures
	dot.setTexture( &stickmanSheet );
	enemy.setTexture( &stickmanUmbrellaSheet );
	enemy1.setTexture( &stickmanUmbrellaSheet );
	enemy2.setTexture( &stickmanUmbrellaSheet );
	enemy3.setTexture( &stickmanUmbrellaSheet );
	enemy4.setTexture( &stickmanUmbrellaSheet );

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
	objects[0] = &dot;
	objects[1] = &enemy;
	objects[2] = &enemy1;
	objects[3] = &enemy2;
	objects[4] = &enemy3;
	objects[5] = &enemy4;

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

			//Handle input for the dot
			( dot.*eventHandler )( e );
		}

//		enemy.walk();
//		enemy1.walk();
//		enemy2.walk();
//		enemy3.walk();
//		enemy4.walk();

		//Time between frames in S
		float frameTime = FPStimer.getMs() / 1000.0f;
		//Restart the timer
		FPStimer.start();

		//Move the dot
		for ( int i = 0; i < objects.size(); i++ ) {
			objects[i]->move( frameTime, objects );
		}

		//Clear screen
		SDL_SetRenderDrawColor( renderer, 100, 110, 100, 0xFF );
		SDL_RenderClear( renderer );

		//Render objects
		for ( int i = 0; i < objects.size(); i++ ) {
			objects[i]->render();
		}

#ifdef DEBUG
		//Render collision boxes
		SDL_SetRenderDrawColor( renderer, 0,0,255, 255);
		for ( int i = 0; i < objects.size(); i++ ) {
			std::vector<BGE_2DVect> box = objects[i]->getCollisionBox();
			SDL_Point points [5];
            for (Uint8 i=0; i<4; i++) {
				points[i] = box[i];	//Implicit cast.
			}
			points[4] = box[0];	//Closed box.
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
	stickmanUmbrellaSheet.free();
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

SDL_Renderer *BGE_Engine::getRenderer() {
	return renderer;
}

TTF_Font *BGE_Engine::getFont() {
	return defaultFont;
}
