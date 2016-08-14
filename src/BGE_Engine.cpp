// Commented includes are reduntant to Game.h

// Definitions for this class
#include "BGE_Engine.h"

// Definitions for other classes of the project
//#include "BGE_Texture.h"
#include <BGE_Object.h>
#include <BGE_Timer.h>
#include <BGE_2DRect.h>
#include <BGE_Tile.h>

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

BGE_Engine::BGE_Engine() {
	//Initialise pointers.
	window = NULL;
	renderer = NULL;
	joystick = NULL;
	BGE_Object::engine = this;
	player = NULL;
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
		window = SDL_CreateWindow( "Culo Culo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );

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

	//Initialise random number generator.
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	randomGen.seed(seed);

	//Initialising game objects data.
	BGE_Object::init();

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
	if( !splintersSheet.loadFromFile( "img/splinters.png" ) ) {
		printf( "Failed to load items splinters!\n" );
		success = false;
	}
	else {
		splintersSheet.setSpriteSize( 25, 25);
		splintersSheet.setSpriteOffset(0, -12);
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
		if ( !textTest.loadFromRenderedText( lines[getRandomInt(0, ctr-1)], 0, 0, 0 ) ) {
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

	//Load level.
	//Load player.
    player = new BGE_Player;
    player->position.x = 50;
    player->position.y = 50;
    player->type = BGE_Object::CREATURE;
    player->material = BGE_Object::Material::FLESH;
    player->texture = &stickmanSheet;

    //Load some random objects.
    for (int i=0; i<30; i++) {
        BGE_Item *item = new BGE_Item;
        item->position.x = getRandomInt(0, 1000);
        item->position.y = getRandomInt(0, 500);
        item->type = static_cast<BGE_Object::Type>(getRandomInt(0, BGE_Object::TOT-1));
        item->material = static_cast<BGE_Object::Material>(getRandomInt(0, static_cast<int>(BGE_Object::Material::TOT)-1));
        item->texture = &itemSheet;
        items.push_back(item);
    }
    //Load some rooms.
    for (int i=0; i<5; i++) {
        loadBuilding(i*8, 0, 7, 7, BGE_Object::Material::PINEWOOD);
    }
	//Load some testing items.
	BGE_Item *item = new BGE_Item;
	item->position.x = 50;
	item->position.y = 200;
	item->type = BGE_Object::BARREL;
	item->material = BGE_Object::Material::PINEWOOD;
	item->texture = &itemSheet;
	items.push_back(item);
	for (int i=0; i<7; i++) {
        BGE_Item *item2 = new BGE_Item;
        item2->position.x = getRandomInt(0, 1000);
        item2->position.y = getRandomInt(0, 500);
        item2->type = BGE_Object::STONE;
        item2->material = BGE_Object::Material::MARBLE;
        item2->texture = &itemSheet;
        items.push_back(item2);
        item->add(item2);
    }
	//dot.setSoundFxs( ouchFx, muoioFx );

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

	//Select controller
	void ( BGE_Player::*eventHandler )( SDL_Event & );

	if ( joystick == NULL ) {
		eventHandler = &BGE_Player::handleEvent;
	}
	else {
		eventHandler = &BGE_Player::handleEventJoy;
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
			( player->*eventHandler )( e );
		}

		//Add and remove objects from lists.
		updateVectors();

		//Time between frames in S
		float frameTime = FPStimer.getMs() / 1000.0f;
		//Restart the timer
		FPStimer.start();

		//Apply speed to objects that move.
		player->update( frameTime );
		for ( int i = 0; i < items.size(); i++ ) {
			items[i]->update( frameTime );
		}
		for ( int i = 0; i < effects.size(); i++ ) {
			effects[i]->update( frameTime );
		}

		//Apply collisions, for each couple of objects applyCollision is called once.
		player->applyCollision(items);
		player->applyCollision(tiles);
		for ( std::vector<BGE_Object*>::iterator i = items.begin(); i < items.end(); i++) {
            std::vector<BGE_Object*> remainingItems (i+1, items.end());
            (*i)->applyCollision( remainingItems );
            (*i)->applyCollision( tiles );
		}

		//Adjust viewport.
		BGE_2DRect freeMove = getFreeMoveArea();
		BGE_2DRect playerBox = player->getCollisionBox();
        if (!freeMove.contains(playerBox)) {
			if (playerBox.x < freeMove.x) {
                viewportOffset.x -= freeMove.x-playerBox.x;
			}
			else if (playerBox.x+playerBox.w > freeMove.x+freeMove.w) {
				viewportOffset.x += playerBox.x+playerBox.w-freeMove.x-freeMove.w;
			}
			if (playerBox.y < freeMove.y) {
                viewportOffset.y -= freeMove.y-playerBox.y;
			}
			else if (playerBox.y+playerBox.h > freeMove.y+freeMove.h) {
				viewportOffset.y += playerBox.y+playerBox.h-freeMove.y-freeMove.h;
			}
        }

        //Update visible object list.
        visibleObjects.clear();
        visibleObjects.push_back(player);
        BGE_2DRect viewport = getViewport();
        for ( int i = 0; i < items.size(); i++ ) {
			BGE_2DRect object = items[i]->getCollisionBox();
            if (viewport.overlaps(object) && items[i]->isVisible()) {
				//Object is inside the viewport and visible.
                visibleObjects.push_back(items[i]);
            }
        }
        for ( int i = 0; i < tiles.size(); i++ ) {
			BGE_2DRect object = tiles[i]->getCollisionBox();
            if (viewport.overlaps(object) && tiles[i]->isVisible()) {
				//Object is inside the viewport and visible.
                visibleObjects.push_back(tiles[i]);
            }
        }
        for ( int i = 0; i < effects.size(); i++ ) {
			BGE_2DRect object = effects[i]->getCollisionBox();
            if (viewport.overlaps(object) && effects[i]->isVisible()) {
				//Object is inside the viewport and visible.
                visibleObjects.push_back(effects[i]);
            }
        }
        std::sort(visibleObjects.begin(), visibleObjects.end(), compareRenderLevel);

		//Clear screen
		SDL_SetRenderDrawColor( renderer, 100, 110, 100, 0xFF );
		SDL_RenderClear( renderer );

		//Render visible objects
		for ( int i = 0; i < visibleObjects.size(); i++ ) {
			visibleObjects[i]->render();
		}

#ifdef DEBUG
		//Render collision boxes
		SDL_SetRenderDrawColor( renderer, 0,0,255, 255);
		for ( int i = 0; i < visibleObjects.size(); i++ ) {
			BGE_2DRect box = visibleObjects[i]->getCollisionBox();
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
	//Free loaded objects.
    delete player;
    for (int i=0; i<items.size(); i++) {
        delete items[i];
        items[i] = NULL;
    }
    items.clear();
    for (int i=0; i<tiles.size(); i++) {
        delete tiles[i];
        tiles[i] = NULL;
    }
    tiles.clear();
    for (int i=0; i<effects.size(); i++) {
        delete effects[i];
        effects[i] = NULL;
    }
    effects.clear();

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

SDL_Renderer *BGE_Engine::getRenderer() {
	return renderer;
}

TTF_Font *BGE_Engine::getFont() {
	return defaultFont;
}

std::vector<BGE_Object *> BGE_Engine::getCollidingObjects() {
	std::vector<BGE_Object *> colliders = items;
	colliders.insert(colliders.end(), tiles.begin(), tiles.end());
	colliders.push_back(player);
	return colliders;
}

void BGE_Engine::add(BGE_Object *object) {
    toAdd.push_back(object);
}

void BGE_Engine::remove(BGE_Object* object) {
	toRemove.push_back(object);
}

void BGE_Engine::updateVectors() {
	//Add new elements to appropriate list.
    for (int i=0; i<toAdd.size(); i++) {
		if (toAdd[i]->type == BGE_Object::TILE) {
            tiles.push_back(toAdd[i]);
		}
		else if (toAdd[i]->type == BGE_Object::SPLINTERS) {
			effects.push_back(toAdd[i]);
		}
		else {
			items.push_back(toAdd[i]);
		}
    }
    toAdd.clear();
    //Remove elements from all lists.
    for (int i=0; i<toRemove.size(); i++) {
		std::vector<BGE_Object *>::iterator index;
		index = std::find(effects.begin(), effects.end(), toRemove[i]);
		if (index != effects.end()) {
			delete *index;
			effects.erase(index);
			continue;
		}
		index = std::find(items.begin(), items.end(), toRemove[i]);
		if (index != items.end()) {
			delete *index;
			items.erase(index);
			continue;
		}
		index = std::find(tiles.begin(), tiles.end(), toRemove[i]);
		if (index != tiles.end()) {
			delete *index;
			tiles.erase(index);
			continue;
		}
		printf("Failed to eliminate: %s\n", toRemove[i]->getName().c_str());
    }
    toRemove.clear();
}

int BGE_Engine::getRandomInt(int min, int max) {
	std::uniform_int_distribution<int> uniformDist (min, max);
	return uniformDist( randomGen);
}

float BGE_Engine::getRandomFloat( float min, float max) {
	std::uniform_real_distribution<float> uniformDist (min, max);
	return uniformDist( randomGen);
}

float BGE_Engine::getNormalRandom( float mean, float stddev) {
	std::normal_distribution<float> normalDist (mean, stddev);
	return normalDist( randomGen);
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
	return front->position.y < back->position.y;
}

void BGE_Engine::loadBuilding(int x, int y, int w, int h, BGE_Object::Material material) {
    for (int i=0; i<h; i++) {
		BGE_Tile *tile = new BGE_Tile;
        tile->position.x = x*BGE_Tile::SIDE;
        tile->position.y = (y+i)*BGE_Tile::SIDE;
        tile->material = material;
        tile->texture = &itemSheet;
        tiles.push_back(tile);
        tile = new BGE_Tile;
        tile->position.x = (x+w-1)*BGE_Tile::SIDE;
        tile->position.y = (y+i)*BGE_Tile::SIDE;
        tile->material = material;
        tile->texture = &itemSheet;
        tiles.push_back(tile);
    }
    for (int i=1; i<w-1; i++) {
		BGE_Tile *tile = new BGE_Tile;
        tile->position.x = (x+i)*BGE_Tile::SIDE;
        tile->position.y = y*BGE_Tile::SIDE;
        tile->material = material;
        tile->texture = &itemSheet;
        tiles.push_back(tile);
        if (i!=1) {
			tile = new BGE_Tile;
			tile->position.x = (x+i)*BGE_Tile::SIDE;
			tile->position.y = (y+h-1)*BGE_Tile::SIDE;
			tile->material = material;
			tile->texture = &itemSheet;
			tiles.push_back(tile);
        }
    }
}
