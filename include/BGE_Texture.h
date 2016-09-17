/*
	Wrapper class for SDL_Texture, used for displaying images and text on screen.
*/

#ifndef GAMETEXTURE_H
#define GAMETEXTURE_H

#include <BGE_2DVect.h>

#include <SDL2/SDL.h>
#include <string>
#include <cstdint>

class BGE_Engine;
struct SDL_Texture;
struct SDL_Rect;
struct SDL_Point;

class BGE_Texture {
	public:
		static BGE_Engine *engine;
		static SDL_Renderer *renderer;

		BGE_Texture();
		~BGE_Texture();

		//Loads image from .png file
		bool loadFromFile( std::string path, int keyRed = -1, int keyGreen = -1, int keyBlue = -1 );
		//Creates image from text using the global font
		bool loadFromRenderedText( std::string textureText, Uint8 red, Uint8 green, Uint8 blue );
		//Creates image from text using the global font
		bool loadFromRenderedTextOnFrame( std::string textureText, Uint8 red, Uint8 green, Uint8 blue);
		//Deallocates texture
		void free();

		//Sets the size of the single sprite in the sheet (default = fullsheet)
		void setSpriteSize( int width, int height);
		//Sets the offset to use when printing the sprite.
		void setSpriteOffset(int x, int y);
		BGE_2DVect getSpriteOffset();

		//Set color modulation
		void setColor( Uint8 red, Uint8 green, Uint8 blue );
		//Set blending
		void setBlendMode( SDL_BlendMode blending );
		//Set alpha modulation
		void setAlpha( Uint8 alpha );

		//Renders whole texture at given point (center)
		void render( float x, float y, SDL_RendererFlip flip = SDL_FLIP_NONE, double angle = 0.0, SDL_Point *center = NULL );
		//Renders sprite from sheet at given point
		void renderSprite( float x, float y, int sheetColumn, int sheetRow, SDL_RendererFlip flip = SDL_FLIP_NONE, double angle = 0.0, SDL_Point *center = NULL );

		//Get image size
		int getWidth();
		int getHeight();

		//The actual hardware texture
		SDL_Texture *texture;
	protected:
	private:
		//Texture size
		int sheetWidth;
		int sheetHeight;
		//Sprite size
		int width;
		int height;
		//Sprite offset
		int offsetX;
		int offsetY;
};

#endif // GAMETEXTURE_H
