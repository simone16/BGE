#include "BGE_World.h"

#include <BGE_Engine.h>
#include <BGE_Tile.h>

BGE_Engine *BGE_World::engine = NULL;

BGE_World::BGE_World() {
    //Piece mask definition:
    //  0 1 2
    //  3 4 5
    //  6 7 8

    //Initialise pieces[]
    pieces[0]  = 0b000000000;    //
    pieces[1]  = 0b000111000;    //-
    pieces[2]  = rotatePiece(pieces[1]);
    pieces[3]  = 0b000110010;    //L
    pieces[4]  = rotatePiece(pieces[3]);
    pieces[5]  = rotatePiece(pieces[4]);
    pieces[6]  = rotatePiece(pieces[5]);
    pieces[7]  = 0b010111000;    //T
    pieces[8]  = rotatePiece(pieces[7]);
    pieces[9]  = rotatePiece(pieces[8]);
    pieces[10] = rotatePiece(pieces[9]);
    pieces[11] = 0b000101000;    //- -
    pieces[12] = rotatePiece(pieces[11]);
    pieces[13] = 0b010111010;    // +
}

BGE_World::~BGE_World() {
    for (int i=0; i<chunks.size(); i++) {
        delete chunks[i];
    }
    chunks.clear();
}

void BGE_World::generateChunk(int x, int y) {
    //Declare new chunk and add it to known ones.
    Chunk *newChunk = new Chunk();
    newChunk->x = x;
    newChunk->y = y;
    chunks.push_back(newChunk);

    //Add some random pieces
    for (int i=0; i<CHUNK_SIZE*CHUNK_SIZE/2; i++) {
        addRandomPiece(newChunk, engine->getRandomInt(0,CHUNK_SIZE-1), engine->getRandomInt(0,CHUNK_SIZE-1) );
    }

    for (int j=0; j<CHUNK_SIZE; j++) {
        for (int i=0; i<CHUNK_SIZE; i++) {
            if (newChunk->getPiece(i, j) == 0) {
                addRandomPiece(newChunk, i, j );
            }
        }
    }


    //Optimise pieces
    for (int opt=0; opt<5; opt++) {
        for (int j=0; j<CHUNK_SIZE; j++) {
            for (int i=0; i<CHUNK_SIZE; i++) {
                optimisePiece(newChunk, i, j);
            }
        }
    }
}

void BGE_World::loadChunk(int x, int y) {
    Chunk *chunk = getChunk(x,y);
    int x0 = x*CHUNK_SIZE*PIECE_SIZE;
    int y0 = y*CHUNK_SIZE*PIECE_SIZE;

    if (chunk != NULL) {
        //Load tiles
        for (int j=0; j<CHUNK_SIZE; j++) {
            for (int i=0; i<CHUNK_SIZE; i++) {
                for (int index=0; index < PIECE_SIZE*PIECE_SIZE; index++) {
                    if ((pieces[chunk->getPiece(i, j)] & (0b1 << index)) > 0) {
                        BGE_Tile *tile = new BGE_Tile( BGE_Object::Material::PINEWOOD );
                        tile->position.x = (i*PIECE_SIZE + (index % PIECE_SIZE) + x0)*BGE_Tile::SIDE;
                        tile->position.y = (j*PIECE_SIZE + (index / PIECE_SIZE) + y0)*BGE_Tile::SIDE;
                        engine->add(tile);
                    }
                }
            }
        }
    }
    else {
        printf("Error! request for chunk (%i, %i): not found.\n");
    }
}

int BGE_World::rotatePiece(int piece) {
    int result = 0;
    int mask = 0b1;
    for (int index=0; index < PIECE_SIZE*PIECE_SIZE; index++) {
        if ((piece & mask) > 0) {
            result += (0b1 << ((index % PIECE_SIZE)*PIECE_SIZE + (PIECE_SIZE-1 - (index/PIECE_SIZE))));
        }
        mask = mask << 1;
    }
    return result;
}

int BGE_World::overlapping(uint8_t piece, Chunk* chunk, int x, int y) {
    //Initialise the score
    int result = 0;

    //Initialise the nearby pieces
    int center, left, right, above, below;
    center = pieces[piece];
    if (x == 0) {
        Chunk *leftChunk = getChunk(chunk->x-1, chunk->y);
        if (leftChunk != NULL) {
            left = pieces[leftChunk->getPiece(CHUNK_SIZE-1, y)];
        }
        else {
            left = 0;
        }
    }
    else {
        left = pieces[chunk->getPiece(x-1, y)];
    }
    if (x == CHUNK_SIZE-1) {
        Chunk *rightChunk = getChunk(chunk->x+1, chunk->y);
        if (rightChunk != NULL) {
            right = pieces[rightChunk->getPiece(0, y)];
        }
        else {
            right = 0;
        }
    }
    else {
        right = pieces[chunk->getPiece(x+1, y)];
    }
    if (y == 0) {
        Chunk *aboveChunk = getChunk(chunk->x, chunk->y-1);
        if (aboveChunk != NULL) {
            above = pieces[aboveChunk->getPiece(x, CHUNK_SIZE-1)];
        }
        else {
            above = 0;
        }
    }
    else {
        above = pieces[chunk->getPiece(x, y-1)];
    }
    if (y == CHUNK_SIZE-1) {
        Chunk *belowChunk = getChunk(chunk->x, chunk->y+1);
        if (belowChunk != NULL) {
            below = pieces[belowChunk->getPiece(x, 0)];
        }
        else {
            below = 0;
        }
    }
    else {
        below = pieces[chunk->getPiece(x, y+1)];
    }

    //Evaluate overlap
    for (int index=0; index<PIECE_SIZE; index++) {
        //Left overlap
        int thisMask = (0b1 << (index*PIECE_SIZE));                     //Start of rows
        int otherMask = (0b1 << (index*PIECE_SIZE + PIECE_SIZE -1));    //end of rows
        if (((left & otherMask) > 0 && (center & thisMask) > 0) ||
            ((left & otherMask) == 0 && (center & thisMask) == 0)) {
            result += 1;
        }

        //Right overlap
        thisMask = (0b1 << (index*PIECE_SIZE + PIECE_SIZE -1));//end of rows
        otherMask = (0b1 << (index*PIECE_SIZE));                //start of rows
        if (((right & otherMask) > 0 && (center & thisMask) > 0) ||
            ((right & otherMask) == 0 && (center & thisMask) == 0)) {
            result += 1;
        }

        //Below overlap
        thisMask = (0b1 << ((PIECE_SIZE-1)*PIECE_SIZE + index));//last row
        otherMask = (0b1 << (index));                           //first row
        if (((below & otherMask) > 0 && (center & thisMask) > 0) ||
            ((below & otherMask) == 0 && (center & thisMask) == 0)) {
            result += 1;
        }

        //Above overlap
        thisMask = (0b1 << (index));                            //first row
        otherMask = (0b1 << ((PIECE_SIZE-1)*PIECE_SIZE + index));//last row
        if (((above & otherMask) > 0 && (center & thisMask) > 0) ||
            ((above & otherMask) == 0 && (center & thisMask) == 0)) {
            result += 1;
        }
    }
    return result;
}

void BGE_World::addRandomPiece( Chunk *chunk, int x, int y) {
    //Calculate scores for each piece
    int probabilities[] = {0,40,40,20,20,20,20,10,10,10,10,20,20,20};
    //TODO consider eventual adjacent chunks
    if (chunk->getPiece(x-1, y) !=0 || chunk->getPiece(x+1, y) !=0 || chunk->getPiece(x, y-1) !=0 || chunk->getPiece(x, y+1) !=0 ) {
        for (int i=0; i<PIECES; i++) {
            probabilities[i] = overlapping(pieces[i], chunk, x, y);
            probabilities[i] = probabilities[i]*probabilities[i]*100;
        }
    }

    //Pick a random piece considering the score of each
    int pieceIndex = 0;
    int maxValue = 0;
    for (int i=0; i<PIECES; i++) {
        maxValue += probabilities[i];
    }
    int randValue = engine->getRandomInt(0, maxValue-1);
    maxValue = 0;
    for (pieceIndex=0; pieceIndex<PIECES; pieceIndex++) {
        maxValue += probabilities[pieceIndex];
        if (randValue < maxValue) {
            break;
        }
    }

    //Add the selected piece to the chunk
    chunk->setPiece( pieceIndex, x, y);
}

void BGE_World::optimisePiece(Chunk *chunk, int x, int y) {
    if (overlapping( chunk->getPiece(x, y), chunk, x, y) <= 4) {
        addRandomPiece(chunk, x, y);
    }
}

void BGE_World::printPiece(uint8_t piece) {
    for (int i=0; i<3; i++) {
        for (int j=0; j<3; j++) {
            if ((0b1<<(j+3*i) & pieces[piece]) > 0) {
                printf("* ");
            }
            else {
                printf("  ");
            }
        }
        printf("\n");
    }
    printf("\n");
}

BGE_World::Chunk* BGE_World::getChunk(int x, int y) {
    for (std::vector<Chunk*>::iterator it = chunks.begin(); it != chunks.end(); it++) {
        if ((*it)->x == x && (*it)->y == y) {
            return (*it);
        }
    }
    return NULL;
}

BGE_World::Chunk::Chunk() {
    for (int i=0; i<CHUNK_SIZE*CHUNK_SIZE; i++) {
        pieces[i] = 0;
    }
    x=0;
    y=0;
}

BGE_World::Chunk::~Chunk() {}

uint8_t BGE_World::Chunk::getPiece(int x, int y) {
    return pieces[x + CHUNK_SIZE*y];
}

void BGE_World::Chunk::setPiece(uint8_t piece, int x, int y) {
    pieces[x + CHUNK_SIZE*y] = piece;
}
