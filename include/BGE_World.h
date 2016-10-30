#ifndef BGE_WORLD_H
#define BGE_WORLD_H

#include <vector>
#include <cstdint>

class BGE_Engine;

class BGE_World {
    public:
        static BGE_Engine *engine;
        static const int CHUNK_SIZE = 50;
        static const int PIECE_SIZE = 3;
        static const int PIECES = 14;

        BGE_World();
        virtual ~BGE_World();

        // Generates randomly a chunk of the world, seamlessly with adjacent chunks.
        void generateChunk(int x, int y);
        // Loads appropriate BGE_Object objs into the engine.
        void loadChunk(int x, int y);

    protected:

        class Chunk {
                public:
                        Chunk();
                        virtual ~Chunk();
                        uint8_t getPiece(int x, int y);
                        void setPiece(uint8_t piece, int x, int y);

                        int x,y;
                private:
                        uint8_t pieces [CHUNK_SIZE*CHUNK_SIZE];
        };

        uint16_t rotatePiece( uint16_t piece);
        //Gives a score from 0 to 8 based on how well the piece suits the given location.
        int overlapping( uint8_t piece, Chunk *chunk, int x, int y );
        void addRandomPiece( Chunk *chunk, int x, int y);
        void optimisePiece( Chunk *chunk, int x, int y);
        void printPiece(uint8_t piece);

        uint16_t pieces[PIECES];
        std::vector<Chunk*> chunks;
    private:
};

#endif // BGE_WORLD_H
