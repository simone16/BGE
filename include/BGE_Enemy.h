#ifndef BGE_ENEMY_H
#define BGE_ENEMY_H

#include <BGE_Creature.h>

class BGE_Enemy : public BGE_Creature {
    public:
        //Threshold used to consider a checkpoint as reached [px]:
        static const float CP_TOLERANCE;

        BGE_Enemy( CreatureType crtType );
        virtual ~BGE_Enemy();

        void update(float Dt);
        void interact(BGE_Object* other, BGE_2DVect overlap);
        void hit(BGE_Object* origin, float energy);
        //Used to tell this that victim has been hit by attacker
        void notifyHit(BGE_Object* attacker, BGE_Creature* victim);

        void addCheckPoint(BGE_2DVect checkPoint);

        // Objects on this list, when seen, will trigger violent behaviour.
        std::vector<BGE_Object*> blackList;
    protected:
        //If not told otherwise, AI navigates the checkpoints.
        std::vector<BGE_2DVect> checkPoints;
        //Points to the checkpoint which is being reached.
        std::vector<BGE_2DVect>::iterator currentCheckPoint;
    private:
};

#endif // BGE_ENEMY_H
