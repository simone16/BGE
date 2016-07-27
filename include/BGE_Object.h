#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <SDL2/SDL.h>
#include <vector>
#include <BGE_2DVect.h>

class BGE_Engine;
struct BGE_Texture;
struct Mix_Chunk;

class BGE_Object {
	public:
		static const int CIRCULAR_COLLISION_PADDING = 2;
		static const float SPEED;

		static BGE_Engine *engine;

		BGE_Object();
		virtual ~BGE_Object();

		virtual void handleEvent( SDL_Event &e ) {};
		virtual void handleEventJoy( SDL_Event &e ) {};

		//Moves the sprite according to it's speed
		virtual void move( float Dt, std::vector<BGE_Object *> others );

		virtual void wound( int damage) {};

		//Renders the object to the screen
		virtual void render();

		//Detects collision based on distance (accurate for circles, very fast).
		virtual bool circularCollision( BGE_Object *other);
		int getCollisionRadius();
		//Detects collision between two rectangles (with/out rotation), slow.
		virtual bool boxCollision( BGE_Object *other, BGE_2DVect *collisionPoint=NULL, BGE_2DVect *correction=NULL);
		std::vector<BGE_2DVect> getCollisionBox();
		static bool hasPointsInside(const std::vector<BGE_2DVect> &rectangularRegion, const std::vector<BGE_2DVect> &pointsToCheck, BGE_2DVect *collisionPoint=NULL, BGE_2DVect *correction=NULL);

		void setTexture( BGE_Texture *texture);
		void setPosition( float x, float y );
		BGE_2DVect getPosition();
		virtual void setSpeed( float x, float y );
	protected:
		//The size of the object (used for collision detection) [px].
		int colliderWidth;
		int colliderHeight;
		int colliderRadius;
		//Position of the sprite (center) [px].
		BGE_2DVect position;
		//Speed of the sprite [px/S].
		BGE_2DVect speed;
		//Direction of the object along x axis [radians].
		float angle;

		//Direction of the sprite.
		SDL_RendererFlip flip;
		//Spritesheet.
		BGE_Texture *texture;
	private:
};

#endif // GAMEOBJECT_H
