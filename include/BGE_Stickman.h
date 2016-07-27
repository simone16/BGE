#ifndef BGE_STICKMAN_H
#define BGE_STICKMAN_H

#include <BGE_Object.h>

class BGE_Stickman : public BGE_Object {
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

        BGE_Stickman();
        virtual ~BGE_Stickman();

        //Assigns snd effects to stickmen
        static void setSoundFxs(Mix_Chunk *wound, Mix_Chunk *die);

        void handleEvent( SDL_Event &e );
		void handleEventJoy( SDL_Event &e );

		void render();

        //Performs a walking routine
		void walk();
		//Reduces HPs.
		void wound( int damage );

		void move(float Dt, std::vector<BGE_Object *> others);

		void setSpeed(float x, float y);
    protected:
    private:
        Status status;
		int animationCtr;
        int healthPoints;
};

#endif // BGE_STICKMAN_H
