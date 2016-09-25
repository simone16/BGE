#ifndef BGE_PANDA_H
#define BGE_PANDA_H

#include <BGE_Creature.h>

class BGE_Player : public BGE_Creature {
    public:
        BGE_Player();
        virtual ~BGE_Player();

        void handleEvent( SDL_Event &e );
		void handleEventJoy( SDL_Event &e );

		void update(float Dt);

		void die();
    protected:
        bool mouseButtonDown;
    private:
};

#endif // BGE_PANDA_H
