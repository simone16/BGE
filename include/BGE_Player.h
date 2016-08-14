#ifndef BGE_PLAYER_H
#define BGE_PLAYER_H

#include <BGE_Item.h>

class BGE_Player : public BGE_Item {
    public:
        //Can carry objects smaller than... [px3].
        static const float POCKETS_VOLUME;
        //Can move objects lighter than... [kg].
        static const float MOVE_WEIGHT_THRESHOLD;
        //Throwing energy [kg*px2/s].
        static const float THROW_ENERGY;
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

		void update(float Dt);
		void interact(BGE_Object * other, BGE_2DVect overlap);
		void use();
		void dispose();

		void move(float Dt, std::vector<BGE_Object *> others);

        //Uses stored mouse position to adjust the angle;
		void updateAngle();

		//Needs to override these to update activeItem.
		void add( BGE_Item *item);
		void remove( BGE_Item *item);
    protected:
    private:
        Status status;
        BGE_Item *activeItem;

        BGE_2DVect mousePositionOnScreen;
};

#endif // BGE_PLAYER_H
