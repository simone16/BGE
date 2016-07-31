#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <SDL2/SDL.h>
#include <vector>
#include <BGE_2DVect.h>
#include <BGE_2DRect.h>
#include <BGE_ObjectType.h>

class BGE_Engine;
class BGE_Texture;
struct Mix_Chunk;

class BGE_Object {
	public:
		static const int CIRCULAR_COLLISION_PADDING = 2;
		static const float SPEED;
		static const float TWO_PI;
		static const float DEGREE_OVER_RADIANS;
		static BGE_Engine *engine;

		//Position of the sprite (center) [px].
		BGE_2DVect position;
		//Speed of the sprite [px/S].
		BGE_2DVect speed;

		BGE_Object();
		virtual ~BGE_Object();

		virtual void handleEvent( SDL_Event &e ) {};
		virtual void handleEventJoy( SDL_Event &e ) {};

		//Moves the sprite according to it's speed.
		virtual void applySpeed( float Dt);
		//Handles collisions.
		//Note it has to be symmetrical (called once for every two objects).
		virtual void applyCollision( std::vector<BGE_Object *> &others );
		//Interact( other) is called whenever this touches other.
		virtual void interact( BGE_Object * other, BGE_2DVect overlapping) {};
		//Tells this obj that it is contained in another.
		void setAsContent(bool content);
		// TODO Possibly eliminate this function and use interact() instead.
		virtual void wound( int damage) {};
		//Used to access content.
		virtual void add( BGE_Object *object);
		virtual void remove( BGE_Object *object);

		//Renders the object to the screen
		virtual void render();

		//Detects collision based on distance (accurate for circles).
		bool circularCollision( BGE_Object *other);
		int getCollisionRadius();
		//Detects collision based on bounding boxes.
		//Applying correction to this position eliminates overlapping.
		bool boxCollision( BGE_Object *other, BGE_2DVect *correction=NULL);
		BGE_2DRect getCollisionBox();

		void setTexture( BGE_Texture *texture);
		void setPosition( float x, float y );
		BGE_2DVect getPosition();
		virtual void setSpeed( float x, float y );
		BGE_2DVect getSpeed();
		//Takes an argument in radians.
		void setAngle(float angle);
		//sets whether to detect collisions for this object.
		void setCollision(bool collides);
		bool canCollide();
		bool isVisible();
		bool isSolid();

		//Properties of this object.
		BGE_ObjectType type;
	protected:
		//The object is rendered if...
		bool visible;
		//The object allows overlapping if...
		bool solid;
		//The size of the object (used for collision detection) [px].
		int colliderWidth;
		int colliderHeight;
		bool collides;
		//Direction of the object along x axis [radians].
		float angle;
		//Objects contained/owned by this.
		std::vector<BGE_Object *> content;


		//sprite flipping direction [none, horizontal, vertical].
		SDL_RendererFlip flip;
		//Spritesheet.
		BGE_Texture *texture;
	private:
};

#endif // GAMEOBJECT_H
