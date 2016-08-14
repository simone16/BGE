#ifndef BGE_OBJECTTYPE_H
#define BGE_OBJECTTYPE_H

#include <BGE_2DRect.h>
#include <string>

class BGE_Engine;

class BGE_ObjectType {
    public:
        static BGE_Engine *engine;

        enum Type : int {
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
            TILE,
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

        //Used to hold the values associated with each type.
        struct TypeData {
            std::string name;
            Use use;        //player.use() section.
            float depth;    //z axis, used for volume [px].
            float width;    //used for collision [px]
            float height;   //used for collision [px]
        };

        Type typeCode;

        BGE_ObjectType(Type _typeCode);
        virtual ~BGE_ObjectType();

        //To be called before using any object of this class.
        static void init();

        void setRandom();

        std::string getName();
        Use getUse();
        float getVolume();
        BGE_2DRect getBox();
        bool canMove();
    protected:
    private:
        static TypeData dataOf[TOT];
};

#endif // BGE_OBJECTTYPE_H
