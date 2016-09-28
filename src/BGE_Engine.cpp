// Definitions for this class
#include "BGE_Engine.h"

// Definitions for other classes in this project
#include <BGE_Enemy.h>
#include <BGE_Player.h>
#include <BGE_Timer.h>
#include <BGE_2DRect.h>
#include <BGE_Tile.h>

// External includes
#include <SDL2/SDL.h>       //SDL library
#include <SDL2/SDL_image.h> //used to load png images (and other formats)
#include <SDL2/SDL_ttf.h>   //used to render text to images/textures
#include <SDL2/SDL_mixer.h> //used to play sounds
#include <stdio.h>
#include <fstream>      //??
#include <chrono>       //used for epoch
#include <algorithm>	//used for std::find and std::sort


// Contains global preprocessor defines.
#include <flags.h>

const float BGE_Engine::FREE_MOVE_ZONE = 0.5;

BGE_Engine::BGE_Engine() {
	//Initialise pointers.
	window = NULL;
	BGE_Texture::renderer = NULL;
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
		window = SDL_CreateWindow( "COPS!!!1!!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );

		if( window == NULL ) {
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else {
			//Create vsynced renderer for window
			BGE_Texture::renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );

			if( BGE_Texture::renderer == NULL ) {
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else {
				//Initialize renderer color
				SDL_SetRenderDrawColor( BGE_Texture::renderer, 0xFF, 0xFF, 0xFF, 0xFF );

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
	std::string filename;
	filename = "img/stickman.png";
	if( !stickmanSheet.loadFromFile( filename) ) {
		printf( "Failed to load %s!\n", filename.c_str());
		success = false;
	}
	else {
		stickmanSheet.setSpriteSize( 25, 50);
		stickmanSheet.setSpriteOffset(0, -12);
	}
	filename = "img/items25x25.png";
	if( !itemSheetSmall.loadFromFile( filename) ) {
		printf( "Failed to load %s!\n", filename.c_str());
		success = false;
	}
	else {
		itemSheetSmall.setSpriteSize( 25, 25);
		itemSheetSmall.setSpriteOffset(0, -12);
	}
	filename = "img/items25x50.png";
	if( !itemSheetTall.loadFromFile( filename) ) {
		printf( "Failed to load %s!\n", filename.c_str());
		success = false;
	}
	else {
		itemSheetTall.setSpriteSize( 25, 50);
		itemSheetTall.setSpriteOffset(0, -12);
	}
	filename = "img/tiles.png";
	if( !tileSheet.loadFromFile( filename) ) {
		printf( "Failed to load %s!\n", filename.c_str());
		success = false;
	}
	else {
		tileSheet.setSpriteSize( 25, 50);
		tileSheet.setSpriteOffset(0, -12);
	}
	filename = "img/splinters.png";
	if( !splintersSheet.loadFromFile( filename) ) {
		printf( "Failed to load %s!\n", filename.c_str());
		success = false;
	}
	else {
		splintersSheet.setSpriteSize( 25, 25);
		splintersSheet.setSpriteOffset(0, -12);
	}
	filename = "img/hats.png";
	if( !hatsSheet.loadFromFile( filename) ) {
		printf( "Failed to load %s!\n", filename.c_str());
		success = false;
	}
	else {
		hatsSheet.setSpriteSize( 25, 25);
		hatsSheet.setSpriteOffset(0, -30);
	}
	filename = "img/textFrame.png";
	if( !textFrame.loadFromFile( filename) ) {
		printf( "Failed to load %s!\n", filename.c_str());;
		success = false;
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
		if ( !textTest.loadFromRenderedTextOnFrame( lines[getRandomInt(0, ctr-1)], 0, 0, 0 ) ) {
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

    //load building
    loadWall(15,-10,10,true,BGE_Object::Material::PINEWOOD);
    loadWall(0,0,17,true,BGE_Object::Material::PINEWOOD);
    loadWall(19,0,16,true,BGE_Object::Material::PINEWOOD);
    loadWall(0,10,27,true,BGE_Object::Material::PINEWOOD);
    loadWall(15,20,3,true,BGE_Object::Material::PINEWOOD);
    loadWall(20,20,7,true,BGE_Object::Material::PINEWOOD);
    loadWall(15,30,20,true,BGE_Object::Material::PINEWOOD);
    loadWall(33,10,2,true,BGE_Object::Material::PINEWOOD);
    loadWall(33,20,2,true,BGE_Object::Material::PINEWOOD);
    loadWall(0,0,10,false,BGE_Object::Material::PINEWOOD);
    loadWall(15,-10,13,false,BGE_Object::Material::PINEWOOD);
    loadWall(25,-10,13,false,BGE_Object::Material::PINEWOOD);
    loadWall(15,5,25,false,BGE_Object::Material::PINEWOOD);
    loadWall(25,5,23,false,BGE_Object::Material::PINEWOOD);
    loadWall(35,0,30,false,BGE_Object::Material::PINEWOOD);
    //check for duplicates
    for (int i=0; i<tiles.size(); i++) {
        for (int j=i+1; j<tiles.size(); j++) {
            if (tiles[i]->position == tiles[j]->position) {
                remove(tiles[i]);
            }
        }
    }
    updateVectors();

    //load enemies...
    BGE_Enemy *enemy = new BGE_Enemy(BGE_Object::CreatureType::COP);
    enemy->position.x = 26*25;
    enemy->position.y = 5*25;
    enemy->target = enemy->position;
    creatures.push_back(enemy);
    //with knifes...
    BGE_Item *item = new BGE_Item(BGE_Object::KNIFE, BGE_Object::Material::IRON);
    items.push_back(item);
    enemy->add(item);
    enemy = new BGE_Enemy(BGE_Object::CreatureType::COP);
    enemy->position.x = 26*25;
    enemy->position.y = 12*25;
    enemy->target = enemy->position;
    creatures.push_back(enemy);
    item = new BGE_Item(BGE_Object::KNIFE, BGE_Object::Material::IRON);
    items.push_back(item);
    enemy->add(item);
    enemy = new BGE_Enemy(BGE_Object::CreatureType::COP);
    enemy->position.x = 33*25;
    enemy->position.y = 12*25;
    enemy->target = enemy->position;
    creatures.push_back(enemy);
    item = new BGE_Item(BGE_Object::KNIFE, BGE_Object::Material::IRON);
    items.push_back(item);
    enemy->add(item);
    enemy = new BGE_Enemy(BGE_Object::CreatureType::COP);
    enemy->position.x = 26*25;
    enemy->position.y = 22*25;
    enemy->target = enemy->position;
    creatures.push_back(enemy);
    //And swords...
    item = new BGE_Item(BGE_Object::SWORD, BGE_Object::Material::IRON);
    items.push_back(item);
    enemy->add(item);
    enemy = new BGE_Enemy(BGE_Object::CreatureType::COP);
    enemy->position.x = 33*25;
    enemy->position.y = 22*25;
    enemy->target = enemy->position;
    creatures.push_back(enemy);
    item = new BGE_Item(BGE_Object::SWORD, BGE_Object::Material::IRON);
    items.push_back(item);
    enemy->add(item);
    enemy = new BGE_Enemy(BGE_Object::CreatureType::COP);
    enemy->position.x = 17*25;
    enemy->position.y = 12*25;
    enemy->target = enemy->position;
    creatures.push_back(enemy);
    //and GUNS!!
    item = new BGE_Item(BGE_Object::GUN, BGE_Object::Material::IRON);
    items.push_back(item);
    enemy->add(item);
    item = new BGE_Item(BGE_Object::BULLETS, BGE_Object::Material::IRON);
    items.push_back(item);
    enemy->add(item);
    enemy = new BGE_Enemy(BGE_Object::CreatureType::COP);
    enemy->position.x = 17*25;
    enemy->position.y = 18*25;
    enemy->target = enemy->position;
    creatures.push_back(enemy);
    item = new BGE_Item(BGE_Object::GUN, BGE_Object::Material::IRON);
    items.push_back(item);
    enemy->add(item);
    item = new BGE_Item(BGE_Object::BULLETS, BGE_Object::Material::IRON);
    items.push_back(item);
    enemy->add(item);
    enemy = new BGE_Enemy(BGE_Object::CreatureType::COP);
    enemy->position.x = 23*25;
    enemy->position.y = 12*25;
    enemy->target = enemy->position;
    creatures.push_back(enemy);
    item = new BGE_Item(BGE_Object::GUN, BGE_Object::Material::IRON);
    items.push_back(item);
    enemy->add(item);
    item = new BGE_Item(BGE_Object::BULLETS, BGE_Object::Material::IRON);
    items.push_back(item);
    enemy->add(item);
    enemy = new BGE_Enemy(BGE_Object::CreatureType::COP);
    enemy->position.x = 23*25;
    enemy->position.y = 18*25;
    enemy->target = enemy->position;
    creatures.push_back(enemy);
    item = new BGE_Item(BGE_Object::GUN, BGE_Object::Material::GOLD);
    items.push_back(item);
    enemy->add(item);
    item = new BGE_Item(BGE_Object::BULLETS, BGE_Object::Material::GOLD);
    items.push_back(item);
    enemy->add(item);

    //load items
    item = new BGE_Item(BGE_Object::KNIFE, BGE_Object::Material::PINEWOOD);
    item->position.x = 17*25;
    item->position.y = -5*25;
    items.push_back(item);
    item = new BGE_Item(BGE_Object::BARREL, BGE_Object::Material::PINEWOOD);
    item->position.x = 18*25;
    item->position.y = -3*25;
    items.push_back(item);
    BGE_Item *cont = new BGE_Item(BGE_Object::GUN, BGE_Object::Material::IRON);
    items.push_back(cont);
    item->add(cont);
    cont = new BGE_Item(BGE_Object::BULLETS, BGE_Object::Material::IRON);
    items.push_back(cont);
    item->add(cont);
    cont = new BGE_Item(BGE_Object::BULLETS, BGE_Object::Material::MARBLE);
    items.push_back(cont);
    item->add(cont);

//    //Load some enemies.
//    for (int i=0; i<5; i++) {
//        BGE_Enemy *enemy = new BGE_Enemy(BGE_Object::CreatureType::COP);
//        enemy->position.x = getRandomInt(0, 1000);
//        enemy->position.y = getRandomInt(0, 500);
//        enemy->target = enemy->position;
//        creatures.push_back(enemy);
//    }
//    BGE_Enemy *walker = new BGE_Enemy(BGE_Object::CreatureType::COWBOY);
//    walker->position.x = -100;
//    walker->position.y = -100;
//    BGE_2DVect point (-300,-100);
//    walker->addCheckPoint(point);
//    point.y = -300;
//    walker->addCheckPoint(point);
//    point.x = -100;
//    point.y = -100;
//    walker->addCheckPoint(point);
//    creatures.push_back(walker);
//
//    //Load some random objects.
//    for (int i=0; i<30; i++) {
//        BGE_Object::Type itemType = static_cast<BGE_Object::Type>(getRandomInt(0, BGE_Object::TOT-1));
//        BGE_Object::Material itemMaterial = static_cast<BGE_Object::Material>(getRandomInt(0, static_cast<int>(BGE_Object::Material::TOT)-1));
//        BGE_Item *item = new BGE_Item(itemType, itemMaterial);
//        item->position.x = getRandomInt(0, 1000);
//        item->position.y = getRandomInt(0, 500);
//        items.push_back(item);
//    }
//	//Load some testing items.
//	BGE_Item *item = new BGE_Item( BGE_Object::BARREL, BGE_Object::Material::PINEWOOD);
//	item->position.x = 50;
//	item->position.y = 200;
//	items.push_back(item);
//	for (int i=0; i<7; i++) {
//        BGE_Item *item2 = new BGE_Item( BGE_Object::STONE, BGE_Object::Material::MARBLE);
//        item2->position.x = getRandomInt(0, 1000);
//        item2->position.y = getRandomInt(0, 500);
//        items.push_back(item2);
//        item->add(item2);
//    }

	return success;
}

void BGE_Engine::start() {
    gameover = false;

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
	while( !gameover ) {
		//Handle events on queue
		while( SDL_PollEvent( &e ) != 0 ) {
			//User requests quit
			if( e.type == SDL_QUIT ) {
				gameover = true;
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

		//Apply speed to objects that move etc...
		player->update( frameTime );
		for ( int i = 0; i < creatures.size(); i++ ) {
			creatures[i]->update( frameTime );
		}
		for ( int i = 0; i < items.size(); i++ ) {
			items[i]->update( frameTime );
		}
		for ( int i = 0; i < effects.size(); i++ ) {
			effects[i]->update( frameTime );
		}

		//Apply collisions, for each couple of objects applyCollision is called once.
		player->applyCollision(creatures);
		player->applyCollision(items);
		player->applyCollision(tiles);
		for ( std::vector<BGE_Object*>::iterator i = creatures.begin(); i < creatures.end(); i++) {
            std::vector<BGE_Object*> remainingCreatures (i+1, creatures.end());
            (*i)->applyCollision( remainingCreatures );
            (*i)->applyCollision( items );
            (*i)->applyCollision( tiles );
		}
		for ( std::vector<BGE_Object*>::iterator i = items.begin(); i < items.end(); i++) {
            std::vector<BGE_Object*> remainingItems (i+1, items.end());
            (*i)->applyCollision( remainingItems );
            (*i)->applyCollision( tiles );
		}
		for ( int i = 0; i < effects.size(); i++ ) {
			effects[i]->applyCollision( tiles );
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
        for ( int i = 0; i < creatures.size(); i++ ) {
			BGE_2DRect object = creatures[i]->getCollisionBox();
            if (viewport.overlaps(object) && creatures[i]->isVisible()) {
				//Object is inside the viewport and visible.
                visibleObjects.push_back(creatures[i]);
            }
        }
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
		SDL_SetRenderDrawColor( BGE_Texture::renderer, 100, 110, 100, 50 );
		SDL_RenderClear( BGE_Texture::renderer );

		//Render visible objects
		for ( int i = 0; i < visibleObjects.size(); i++ ) {
			visibleObjects[i]->render();
		}

#ifdef DEBUG
		//Render collision boxes
		SDL_SetRenderDrawColor( BGE_Texture::renderer, 0,0,255, 255);
		for ( int i = 0; i < visibleObjects.size(); i++ ) {
			BGE_2DRect box = visibleObjects[i]->getCollisionBox();
			box -= viewportOffset;
			SDL_Point points [5];
            points[0] = {box.x,box.y};
            points[1] = {box.x+box.w,box.y};
            points[2] = {box.x+box.w,box.y+box.h};
            points[3] = {box.x,box.y+box.h};
            points[4] = {box.x,box.y};
			SDL_RenderDrawLines(BGE_Texture::renderer, points, 5);
		}
#endif // DEBUG

		//Render test text
		textTest.render( SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 );

		//Render player data
		SDL_Rect progressBar = {50, SCREEN_HEIGHT-50, 0, 10};
        progressBar.w = player->getHealthPercent();
        SDL_SetRenderDrawColor( BGE_Texture::renderer, 255, 50, 50, 200);
        SDL_RenderFillRect( BGE_Texture::renderer, &progressBar);
        SDL_SetRenderDrawColor( BGE_Texture::renderer, 50, 50, 200, 200);
        progressBar.y += 20;
        progressBar.w = 100-player->getUseDelayPercent();
        SDL_RenderFillRect( BGE_Texture::renderer, &progressBar);

		//Update screen
		SDL_RenderPresent( BGE_Texture::renderer );
	}
}

void BGE_Engine::close() {
	//Free loaded objects.
    delete player;
    for (int i=0; i<creatures.size(); i++) {
        delete creatures[i];
        creatures[i] = NULL;
    }
    creatures.clear();
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
	itemSheetSmall.free();
	splintersSheet.free();
	hatsSheet.free();
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
	SDL_DestroyRenderer( BGE_Texture::renderer );
	SDL_DestroyWindow( window );
	window = NULL;
	BGE_Texture::renderer = NULL;

	//Quit SDL subsystems
	Mix_Quit();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

void BGE_Engine::gameOver() {
    gameover = true;
}

TTF_Font *BGE_Engine::getFont() {
	return defaultFont;
}

BGE_Player *BGE_Engine::getPlayer() {
    return player;
}

std::vector<BGE_Enemy *> BGE_Engine::getAICreatures() {
	std::vector<BGE_Enemy *> list (creatures.size());
	for (int i=0; i<creatures.size(); i++) {
        list[i] = static_cast<BGE_Enemy*>(creatures[i]);
	}
	return list;
}

std::vector<BGE_Object *> BGE_Engine::getCollidingObjects() {
	std::vector<BGE_Object *> colliders = creatures;
	colliders.insert(colliders.end(), items.begin(), items.end());
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
		else if (toAdd[i]->type == BGE_Object::PARTICLE) {
			effects.push_back(toAdd[i]);
		}
		else if (toAdd[i]->type == BGE_Object::CREATURE) {
            creatures.push_back(toAdd[i]);
		}
		else {
			items.push_back(toAdd[i]);
		}
    }
    toAdd.clear();
    //Remove elements from all lists.
    for (int i=0; i<toRemove.size(); i++) {
		std::vector<BGE_Object *>::iterator index;
		index = std::find(creatures.begin(), creatures.end(), toRemove[i]);
		if (index != creatures.end()) {
			delete *index;
			creatures.erase(index);
			continue;
		}
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

void BGE_Engine::loadWall(int x, int y, int lenght, bool horizontal, BGE_Object::Material material) {
    int *increment = &y;
    if (horizontal) {
        increment = &x;
    }
    for (int i=0; i<lenght; i++) {
        BGE_Tile *tile = new BGE_Tile( material );
        tile->position.x = x*BGE_Tile::SIDE;
        tile->position.y = y*BGE_Tile::SIDE;
        tiles.push_back(tile);
        (*increment)++;
    }
}
