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
		virtual void hit(BGE_Object* origin, float energy);
		//The following refer to activeItem.
		virtual void use();
		void dispose();

        //Uses stored mouse position to adjust the angle;
		void updateAngle();

		//Needs to override these to update activeItem.
		void add( BGE_Item *item);
		void remove( BGE_Item *item);

		bool canSee(BGE_Object* target);
		float getUseDelayPercent();
		float getMaxHealth();
		float getViewField();
		std::string getName();

		BGE_2DVect target;

		CreatureType creatureType;
    protected:
        //Points to a contained item.
        BGE_Item *activeItem;
        //Time remaining before use() can be called again [S].
        float useDelay;
        //Main sprite animation counter.
        int animCtr;
        //Weapon sprite animation counter.
        int weaponAnimCtr;

        CreatureData getCreatureData();
    private:
};

#endif // BGE_PLAYER_H
