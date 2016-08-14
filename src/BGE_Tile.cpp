#include "BGE_Tile.h"

#include <BGE_Texture.h>

#include <stdio.h>

const float BGE_Tile::SIDE = 25.0;

BGE_Tile::BGE_Tile() {
    type = BGE_Object::TILE;
}

BGE_Tile::~BGE_Tile() {}


void BGE_Tile::render() {
    texture->renderSprite(position.x, position.y, static_cast<int>(material), 2);
}
