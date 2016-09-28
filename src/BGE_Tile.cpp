#include "BGE_Tile.h"

#include <BGE_Engine.h>
#include <BGE_Texture.h>

#include <stdio.h>

#include <flags.h>

const float BGE_Tile::SIDE = 25.0;

BGE_Tile::BGE_Tile( Material tileMaterial):
    BGE_Object(BGE_Object::TILE, tileMaterial) {
}

BGE_Tile::~BGE_Tile() {}

void BGE_Tile::render() {
    SDL_Color color = getColor();
    engine->tileSheet.setColor(color.r, color.g, color.b);
    engine->tileSheet.renderSprite(position.x, position.y, getMaterialData().tileMaskIndex, 0);
}
