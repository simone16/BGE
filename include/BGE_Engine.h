/*
    This class uses SDL to display a window with graphics and sounds and handle input-events.
    The main function should just call init, load, start and close in this order.
    Level advancement, interactions and other high level behaviours should be implemented in start().
    Lower level behaviours of the charachters and objects in the game should be implemented in GameObject class.
*/

#ifndef GAME_H
#define GAME_H

#include <vector>
#include "BGE_Texture.h"
#include "BGE_Player.h"
#include "BGE_Item.h"
#include <BGE_2DRect.h>
#include <BGE_2DVect.h>

#include <random>

class BGE_Object;

struct SDL_Window;
struct SDL_Renderer;
struct Mix_Chunk;
struct _TTF_Font;
typedef _TTF_Font TTF_Font;

class BGE_Engine {
	public:
		const static int SCREEN_WIDTH = 640;
		const static int SCREEN_HEIGHT = 480;
		const static float FREE_MOVE_ZONE;
		const static int JOYSTICK_DEAD_ZONE = 8000;

		BGE_Engine();
		virtual ~BGE_Engine();

		//Preps SDL for usage.
		bool init();

		//Reads external resources (text, images and audio) to ram.
		bool load();

		//Executes the game itself.
		void start();

		//Once the user quits the game, this function has to be called.
		//Mainly used to release memory (complementary to load()).
		void close();

		//Handle the rendering list. (TODO unused)
		void show(BGE_Object *object);
		void hide(BGE_Object *object);
		//Returns the renderer to use for displaying textures on screen.
		SDL_Renderer * getRenderer();
		//Returns the default font of the game (used to create textures from text).
		TTF_Font * getFont();
		//Returns the offset to apply to sprites positions.
		BGE_2DVect getViewportOffset();

		//Returns a list of all the loaded objects.
        std::vector<BGE_Object *> getOthers();

		int getRandom(int min, int max);
	protected:
	private:
		SDL_Window *window;
		SDL_Renderer *renderer;
		SDL_Joystick *joystick;
		TTF_Font *defaultFont;
		bool useJoystick;
		BGE_2DVect viewportOffset;

		BGE_Texture stickmanSheet, itemSheet;
		BGE_Texture textTest;
		Mix_Chunk *ouchFx, *muoioFx;

		BGE_Player dot;
		BGE_Item it1, it2, it3, it4, it5;
		//Objects present in the level (TODO will be vector<BGE_Object>)
		std::vector<BGE_Object *> loadedObjects;
		//Objects inside the current viewport.
		std::vector<BGE_Object *> visibleObjects;

		std::default_random_engine randomGen;

		BGE_2DRect getFreeMoveArea();
		BGE_2DRect getViewport();
		static bool compareRenderLevel(BGE_Object *front, BGE_Object *back);
};

#endif // GAME_H
