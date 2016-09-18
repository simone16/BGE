#ifndef BGE_ENEMY_H
#define BGE_ENEMY_H

#include <BGE_Creature.h>

class BGE_Enemy : public BGE_Creature {
    public:
        BGE_Enemy( CreatureType crtType );
        virtual ~BGE_Enemy();

        void update(float Dt);
        void interact(BGE_Object* other, BGE_2DVect overlap);

        void addCheckPoint(BGE_2DVect checkPoint);

    protected:
        //If not told otherwise, AI navigates the checkpoints (avoiding obstacles).
        std::vector<BGE_2DVect> checkPoints;
        //Points to the checkpoint which is being reached.
        std::vector<BGE_2DVect>::iterator currentCheckPoint;
    private:
};

#endif // BGE_ENEMY_H
