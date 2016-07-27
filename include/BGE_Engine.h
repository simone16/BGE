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
#include "BGE_Stickman.h"

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

		//Returns the renderer to use for displaying textures on screen.
		SDL_Renderer * getRenderer();
		//Returns the default font of the game (used to create textures from text).
		TTF_Font * getFont();
	protected:
	private:
		SDL_Window *window;
		SDL_Renderer *renderer;
		SDL_Joystick *joystick;
		TTF_Font *defaultFont;
		bool useJoystick;

		BGE_Texture stickmanSheet, stickmanUmbrellaSheet;
		BGE_Texture textTest;
		Mix_Chunk *ouchFx, *muoioFx;

		BGE_Stickman dot;
		BGE_Stickman enemy, enemy1, enemy2, enemy3, enemy4;
		std::vector<BGE_Object *> objects;
};

#endif // GAME_H
