#ifndef BGE_MOVINGOBJECT_H
#define BGE_MOVINGOBJECT_H

#include <BGE_Object.h>


class BGE_Mover : public BGE_Object {
    public:
        BGE_Mover();
        virtual ~BGE_Mover();

        //Moves the object according to it's speed.
        virtual void update(float Dt);
        //Moves away from the origin.
        virtual void hit(BGE_2DVect origin, float energy);

        void setAsContent(bool content);

        BGE_2DVect speed;
    protected:

    private:
};

#endif // BGE_MOVINGOBJECT_H
