#ifndef BGE_ITEM_H
#define BGE_ITEM_H

#include <BGE_Object.h>
#include <BGE_2DVect.h>

class BGE_Item : public BGE_Object {
    public:
        //The fraction of speed loss 0<DRAG<1 [1/s].
        static const float DRAG;
        //Speed bottom limit to speed=0 [px/s].
        static const float STOP_THRESHOLD;

        BGE_Item(Type objectType, Material objMaterial);
        virtual ~BGE_Item();

        enum class MovementLock: uint8_t {
                HORIZONTAL,
                VERTICAL,
                BOTH,
                NONE
        };

        //Called at every computational step.
        virtual void update(float Dt);
        //Called when collision is detected with other.
        virtual void interact( BGE_Object * other, BGE_2DVect overlap);
        //Called when this is hit.
        virtual void hit(BGE_2DVect origin, float energy);
        //Used to eliminate object.
        virtual void die();
        //Tells this item that it is contained in another.
        virtual void setAsContent(bool content);
        //Used to modify content.
        virtual void add( BGE_Item *item);
        virtual void remove( BGE_Item *item);

        virtual void render();

        virtual void setAngle(float angle);

        //Current speed.
        BGE_2DVect speed;
        MovementLock lock;
    protected:

        //Objects contained/owned by this.
        std::vector<BGE_Item *> content;
    private:
};

#endif // BGE_ITEM_H
