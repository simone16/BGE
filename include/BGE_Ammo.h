#ifndef BGE_AMMO_H
#define BGE_AMMO_H

#include <BGE_Item.h>


class BGE_Ammo : public BGE_Item
{
    public:
        BGE_Ammo();
        virtual ~BGE_Ammo();

    protected:
    private:
        Uint8 remainingSheels;
};

#endif // BGE_AMMO_H
