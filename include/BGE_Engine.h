/*
    This class uses SDL to display a window with graphics and sounds and handle input-events.
    The main function should just call init, load, start and close in this order.
    Level advancement, interactions and other high level behaviours should be implemented in start().
    Lower level behaviours of the charachters and objects in the game should be implemented in GameObject class.
*/

#ifndef GAME_H
#define GAME_H

#include <vector>
#include <BGE_Texture.h>
#include <BGE_Object.h>
#include <BGE_2DRect.h>
#include <BGE_2DVect.h>
#include <flags.h>

#include <random>

class BGE_Creature;
class BGE_Enemy;
class BGE_Player;

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

		void gameOver();

		//Returns the default font of the game (used to create textures from text).
		TTF_Font * getFont();
		//Returns the offset to apply to sprites positions.
		BGE_2DVect getViewportOffset();

		BGE_Player * getPlayer();
		//Returns a list of the creatures loaded.
		std::vector<BGE_Enemy *> getAICreatures();
		//Returns a list of loaded collideable objects.
        std::vector<BGE_Object *> getCollidingObjects();
        //Returns a list of objects implementing Moveable.
        std::vector<BGE_Object *> getMoveableObjects();
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

		//TODO organise better
		void playCreature();
		void playItem();
		void playWeapon();

		//Game textures
		BGE_Texture stickmanSheet;
		BGE_Texture itemSheetSmall;
		BGE_Texture itemSheetTall;
		BGE_Texture tileSheet;
		BGE_Texture hatsSheet;
		BGE_Texture particlesSheet;
	protected:
	private:
		bool gameover;

		SDL_Window *window;
		SDL_Joystick *joystick;
		TTF_Font *defaultFont;
		bool useJoystick;
		BGE_2DVect viewportOffset;

		BGE_Texture textTest;
		//Game snd efx
		std::vector<Mix_Chunk *> creatureHitFxs;
		std::vector<Mix_Chunk *> itemFxs;
		std::vector<Mix_Chunk *> weaponFxs;

		//Player in the current level.
		BGE_Player * player;
		std::vector<BGE_Object *> creatures;
		std::vector<BGE_Object *> items;
		std::vector<BGE_Object *> tiles;
		std::vector<BGE_Object *> effects;
		//Objects inside the current viewport.
		std::vector<BGE_Object *> visibleObjects;
		//Used to manipulate objects at runtime.
		std::vector<BGE_Object *> toAdd;
		std::vector<BGE_Object *> toRemove;

		std::default_random_engine randomGen;

		BGE_2DRect getFreeMoveArea();
		BGE_2DRect getViewport();
		static bool compareRenderLevel(BGE_Object *front, BGE_Object *back);

		void loadWall(int x, int y, int lenght, bool horizontal, BGE_Object::Material material);
};

#endif // GAME_H
