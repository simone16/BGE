#ifndef BGE_TILE_H
#define BGE_TILE_H

#include <BGE_Object.h>


class BGE_Tile : public BGE_Object {
    public:
        static const float SIDE;

        BGE_Tile();
        virtual ~BGE_Tile();

        void render();

    protected:

    private:
};

#endif // BGE_TILE_H
