#ifndef BGE_PLAYER_H
#define BGE_PLAYER_H

#include <BGE_Item.h>

class BGE_Creature : public BGE_Item {
    public:
        //Can carry objects smaller than... [px3].
        static const float POCKETS_VOLUME;
        //Can move objects lighter than... [kg].
        static const float MOVE_WEIGHT_THRESHOLD;
        //Throwing energy [kg*px2/s].
        static const float THROW_ENERGY;
        //-----------------------------------------the above will (hopefully) dep. on stats.
        //Number of walking frames & repetition.
        static const int WALK_FRAMES = 12;
        static const int WALK_FRAME_REPEAT = 3;

        BGE_Creature( CreatureType crtType );
        virtual ~BGE_Creature();

		virtual void render();

		virtual void update(float Dt);
		virtual void interact(BGE_Object * other, BGE_2DVect overlap);
		virtual void hit(BGE_2DVect origin, float energy);
		//The following refer to activeItem.
		void use();
		void dispose();

        //Uses stored mouse position to adjust the angle;
		void updateAngle();

		//Needs to override these to update activeItem.
		void add( BGE_Item *item);
		void remove( BGE_Item *item);

		float getMaxHealth();
		std::string getName();

		BGE_2DVect target;

		CreatureType creatureType;
    protected:
        //Points to a contained item.
        BGE_Item *activeItem;
        //Weapon sprite animation counter.
        int weaponAnimCtr;
        //Time remaining before use() can be called again [S].
        float useDelay;

        //Detects the first object on a sigle segment path.
        bool segmentCollision(BGE_2DVect start, BGE_2DVect end, BGE_Object **firstCollision, BGE_2DVect *collision);
    private:
};

#endif // BGE_PLAYER_H
