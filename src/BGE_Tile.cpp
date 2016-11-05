#include "BGE_Tile.h"

#include <BGE_Engine.h>
#include <BGE_Texture.h>

#include <stdio.h>

#include <flags.h>

const float BGE_Tile::SIDE = 25.0;

int BGE_Tile::getMaskIndex(int matIndex) {
    return dataOfMaterial[matIndex].tileMaskIndex;
}

SDL_Color BGE_Tile::getTileColor(int matIndex) {
    return dataOfMaterial[matIndex].color;
}

BGE_Tile::BGE_Tile( Material tileMaterial):
    BGE_Object(BGE_Object::TILE, tileMaterial) {
}

BGE_Tile::BGE_Tile( Serialized data):
    BGE_Object( BGE_Object::TILE, static_cast<BGE_Object::Material>(data.material)) {
    health = data.health;
    position.x = data.x;
    position.y = data.y;
}

BGE_Tile::~BGE_Tile() {}

void BGE_Tile::render() {
    engine->tileSheet.renderSprite(position.x, position.y, static_cast<int>(material), 0);
}

BGE_Tile::Serialized BGE_Tile::serialize() {
    Serialized data;
    data.health = health;
    data.material = static_cast<int>(material);
    data.type = static_cast<int>(type);
    data.x = position.x;
    data.y = position.y;
    return data;
}
