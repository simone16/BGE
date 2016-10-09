#ifndef BGE_ITEM_H
#define BGE_ITEM_H

#include <BGE_Object.h>
#include <BGE_Moveable.h>
#include <BGE_2DVect.h>

class BGE_Item : public BGE_Moveable {
    public:
        BGE_Item(Type objectType, Material objMaterial);
        virtual ~BGE_Item();

        //Called at every computational step.
        virtual void update(float Dt);
        //Called when collision is detected with other.
        virtual void interact( BGE_Object * other, BGE_2DVect overlap);
        //Called when this is hit.
        virtual void hit(BGE_Object* origin, float energy);
        //Overridden to handle content.
        virtual void die();
        //Tells this item that it is contained in another.
        virtual void setAsContent(bool content);
        //Used to modify content.
        virtual void add( BGE_Item *item);
        virtual void remove( BGE_Item *item);

        virtual void render();

        virtual float getDamage();
        bool selfDestruct;
    protected:

        //Objects contained/owned by this.
        std::vector<BGE_Item *> content;
    private:
};

#endif // BGE_ITEM_H
