#ifndef BGE_PLAYER_H
#define BGE_PLAYER_H

#include <BGE_Object.h>

class BGE_Player : public BGE_Object {
    public:
        // How many times each animation frame is repeated.
        static const int FRAME_REPEAT = 5;
        static Mix_Chunk *woundFx;
		static Mix_Chunk *dieFx;

        // Used to represent the current status of the stickman.
        enum Status {
			IDLE,
			WALKING,
			ATTACK,
			DIE,
			WOUNDED
		};

        BGE_Player();
        virtual ~BGE_Player();

        //Assigns snd effects to stickmen
        static void setSoundFxs(Mix_Chunk *wound, Mix_Chunk *die);

        void handleEvent( SDL_Event &e );
		void handleEventJoy( SDL_Event &e );

		void render();

        //Performs a walking routine
		void walk();

		void applySpeed(float Dt);
		void interact(BGE_Object * other, BGE_2DVect overlap);
		void use(BGE_Object *object);
		//Reduces HPs.
		void wound( int damage );

		void move(float Dt, std::vector<BGE_Object *> others);

		void setSpeed(float x, float y);
        //Uses stored mouse position to adjust the angle;
		void updateAngle();

		//Needs to override these to update activeItem.
		void add( BGE_Object *object);
		void remove( BGE_Object *object);
    protected:
    private:
        Status status;
        float healthPoints;
        BGE_Object *activeItem;

        BGE_2DVect mousePositionOnScreen;

        Uint8 animationCtr;
        Uint8 attackAnimationCtr;
};

#endif // BGE_STICKMAN_H
