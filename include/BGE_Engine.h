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

		//Reads external resources (text, images and audio) and level.
		bool load();

		//Executes the game itself.
		void start();

		//Once the user quits the game, this function has to be called.
		//Mainly used to release memory (complementary to load()).
		void close();

		//Returns the renderer to use for displaying textures on screen.
		SDL_Renderer * getRenderer();
		//Returns the default font of the game (used to create textures from text).
		TTF_Font * getFont();
		//Returns the offset to apply to sprites positions.
		BGE_2DVect getViewportOffset();

		//Returns a list of loaded collideable objects.
        std::vector<BGE_Object *> getCollidingObjects();
        //Add an object to the current level.
        void add(BGE_Object * object);
        //Removes an object from the current level.
        void remove(BGE_Object * object);
        //Applies the changes queued with add and remove.
        void updateVectors();

        //Functions for random numbers generation.
		int getRandomInt(int min, int max);
		float getRandomFloat(float min, float max);
		float getNormalRandom(float mean, float stddev);

		 BGE_Texture splintersSheet;
		 BGE_Texture stickmanSheet, itemSheet;
	protected:
	private:
		SDL_Window *window;
		SDL_Renderer *renderer;
		SDL_Joystick *joystick;
		TTF_Font *defaultFont;
		bool useJoystick;
		BGE_2DVect viewportOffset;

		BGE_Texture textTest;
		Mix_Chunk *ouchFx, *muoioFx;

		//Player in the current level.
		BGE_Player * player;
		std::vector<BGE_Object *> items;
		std::vector<BGE_Object *> tiles;
		std::vector<BGE_Object *> effects;
		//Objects inside the current viewport.
		std::vector<BGE_Object *> visibleObjects;
		//Used to manipulate objects in runtime.
		std::vector<BGE_Object *> toAdd;
		std::vector<BGE_Object *> toRemove;

		std::default_random_engine randomGen;

		BGE_2DRect getFreeMoveArea();
		BGE_2DRect getViewport();
		static bool compareRenderLevel(BGE_Object *front, BGE_Object *back);

		void loadBuilding(int x, int y, int w, int h, BGE_Object::Material material);
};

#endif // GAME_H
