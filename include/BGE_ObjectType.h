#ifndef BGE_OBJECTTYPE_H
#define BGE_OBJECTTYPE_H

#include <BGE_ObjectMaterial.h>

class BGE_Engine;

class BGE_ObjectType {
    public:
        static BGE_Engine *engine;

        enum class Type : int {
            BARREL,
            BLOCK,
            BOTTLE,
            BULLETS,
            CAR,
            CHAIR,
            GUN,
            CREATURE,
            MOUSE,
            KNIFE,
            STONE,
            SWORD,
            TABLE,
            TOT
        };

        //Use determines what Player::use does
        enum class Use : int {
            WEAPON,         //Throw it.
            HANDHELD_WEAPON,//Spawn in front of player and then hide again, wound nearby creatures.
            SHOOTING_WEAPON,//As above, but cause another object to be thrown.
            FOOD,           //Eat.
            FURNITURE,      //Place in front of player.
            TRANSPORTATION, //Get inside.
            NONE            //Not directly usable.
        };

        Type type;
        BGE_ObjectMaterial material;

        BGE_ObjectType();
        virtual ~BGE_ObjectType();

        void setRandom();
        Type getType();
        Use getUse();
        BGE_ObjectMaterial getMaterial();
        float getVolume();
        float getMass();
        float getToxicity();
        bool canMove();
        std::string getName();
        std::string getTypeName();
    protected:
    private:
};

#endif // BGE_OBJECTTYPE_H
