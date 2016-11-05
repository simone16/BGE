#ifndef BGE_TILE_H
#define BGE_TILE_H

#include <BGE_Object.h>


class BGE_Tile : public BGE_Object {
    public:
        static const float SIDE;

        struct Serialized {
            int type;
            int material;
            float health;
            float x;
            float y;
        };

        //Used to generate tile sprites.
        static int getMaskIndex( int matIndex);
        static SDL_Color getTileColor( int matIndex);

        BGE_Tile( Material tileMaterial);
        BGE_Tile( Serialized data);
        virtual ~BGE_Tile();

        void render();

        Serialized serialize();

    protected:

    private:
};

#endif // BGE_TILE_H
