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

        BGE_Item();
        virtual ~BGE_Item();

        void applySpeed(float Dt);

        void interact( BGE_Object * other, BGE_2DVect overlap);

        void render();
    protected:
    private:
};

#endif // BGE_ITEM_H
