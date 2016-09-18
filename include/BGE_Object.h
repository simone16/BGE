#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <BGE_2DVect.h>
#include <BGE_2DRect.h>

#include <SDL2/SDL.h>
#include <vector>
#include <string>

class BGE_Engine;
class BGE_Texture;
struct Mix_Chunk;

class BGE_Object {
	public:
		static const int CIRCULAR_COLLISION_PADDING = 2;
		static const float SPEED;	//TODO what's this?
		static const float TWO_PI;
		static const float DEGREE_OVER_RADIANS;
		static const float LITERS_PER_CUBIC_PX;
		static BGE_Engine *engine;

		//Types of objects available.
		enum Type : int {
            BARREL,
            BLOCK,
            BOTTLE,
            BULLETS,
            CHAIR,
            GUN,
            CREATURE,
            MOUSE,
            KNIFE,
            STONE,
            SWORD,
            VIS_EFFECT,
            TABLE,
            TILE,
            TOT
        };
        //Use determines what Player::use does
        enum class Use : int {
            WEAPON,         //Throw it.
            HANDHELD_WEAPON,//Spawn in front of player and then hide again, wound nearby creatures.
            SHOOTING_WEAPON,//As above, but cause another object to be thrown.
            FOOD,           //Eat.
            TRANSPORTATION, //Get inside.
            NONE            //Not directly usable.
        };
        //Used to hold the values associated with each type of object.
        struct TypeData {
            std::string name;
            Use use;        //player.use() action.
            float reloadTime;//min delay between use() calls [S].
            float baseHealth;//starting health [HP].
            float depth;    //z axis, used for volume [px].
            float width;    //used for collision [px]
            float height;   //used for collision [px]
            BGE_Texture *sheet;//sheet with this sprite
            int column; //location of the sprite on the sheet
        };

        //Object materials available.
        enum class Material : int {
			FLESH,
			BONE,
			IRON,
			PINEWOOD,
			MARBLE,
			FIBERGLASS,
			DRUG,
			WATER,
			FABRIC,
			GOLD,
			TOT
		};
		//State associated with each material.
        enum class PhysicalState : int {
			HARD_SOLID,
			SOFT_SOLID,
			LIQUID,
			GAS,
			TOT
		};
		//Used to hold the values associated with each material.
        struct MaterialData {
            std::string name;
            PhysicalState state;
            float density;       // [kg/px3]
            float nutrition;     // [HP/kg] negative -> toxic.
            float strenght;      // [HP/HP]
        };

        //Types of creatures available.
		enum class CreatureType : int {
            COP,
            PLAYER,
            COWBOY,
            SHERLOK,
            TOT
        };
        //Used to hold values associated with each creature.
        struct CreatureData {
            std::string name;
            float health;
            int spriteIndex;    //used to find hats.
        };

        //To be called before using any object of this class (or derived classes).
        static void init();

		BGE_Object( Type objectType, Material objMaterial);
		virtual ~BGE_Object();

		virtual void update(float Dt);
		virtual void hit( BGE_2DVect origin, float energy);
		//Handles collisions.
		//Note it has to be symmetrical (called once for every two objects, see engine.start()).
		virtual void applyCollision( std::vector<BGE_Object *> &others );
		//Interact( other) is called whenever this touches other.
		virtual void interact( BGE_Object * other, BGE_2DVect overlapping) {};
		//Displays a message on top of the object.
		virtual void say(std::string messageTexture);
		//Used to eliminate object.
		virtual void die();

		//Renders the object to the screen
		virtual void render();

		//Detects collision based on distance (accurate for circles).
		bool circularCollision( BGE_Object *other);
		float getCollisionRadius();
		//Detects collision based on bounding boxes.
		//Applying correction to this position eliminates overlapping.
		bool boxCollision( BGE_Object *other, BGE_2DVect *correction=NULL);
		BGE_2DRect getCollisionBox();

		//Takes an argument in radians.
		void setAngle(float angle);
		//sets whether to detect collisions for this object.
		void setCollision(bool collides);
		bool canCollide();
		bool isVisible();
		bool isSolid();
		float getHealthPercent();

		//Data derived from object type and/or material.
		Use getUse();
		float getVolume();
		float getDepth();
		float getMass();
		float getNutrition();
		virtual float getMaxHealth();
		PhysicalState getPhysicalState();
		float getReloadTime();
		bool canMove();
		virtual std::string getName();

		//Position of the object (center) [px].
		BGE_2DVect position;
		//Properties of this object.
		Type type;
		Material material;
	protected:
		//The object is rendered if...
		bool visible;
		//The object allows overlapping if not...
		bool solid;
		//Detect collision with other objects if both...
		bool collides;
		//Direction of the object along x axis [radians].
		float angle;
		float health;

        //sprite flipping direction [none, horizontal, vertical].
		SDL_RendererFlip flip;
		//Sprite animation counter.
		int animCtr;
		//Current Message texture.
		BGE_Texture *messageTexture;
		//Time to display the message [S].
		float messageTimer;

		//Data relative to object type.
		static TypeData dataOf[];
		//Data relative to object material.
		static MaterialData dataOfMaterial[];
		//Data relative to creatures.
		static CreatureData dataOfCreature[];
	private:
};

#endif // GAMEOBJECT_H
