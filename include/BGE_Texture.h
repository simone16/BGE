/*
	Wrapper class for SDL_Texture, used for displaying images and text on screen.
*/

#ifndef GAMETEXTURE_H
#define GAMETEXTURE_H

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

		BGE_Texture();
		~BGE_Texture();

		//Loads image from .png file
		bool loadFromFile( std::string path, int keyRed = -1, int keyGreen = -1, int keyBlue = -1 );
		//Creates image from text using the global font
		bool loadFromRenderedText( std::string textureText, Uint8 red, Uint8 green, Uint8 blue );
		//Deallocates texture
		void free();

		//Sets the size of the single sprite in the sheet (default = fullsheet)
		void setSpriteSize( int width, int height);

		//Set color modulation
		void setColor( Uint8 red, Uint8 green, Uint8 blue );
		//Set blending
		void setBlendMode( SDL_BlendMode blending );
		//Set alpha modulation
		void setAlpha( Uint8 alpha );

		//Renders whole texture at given point (center)
		void render( int x, int y, SDL_RendererFlip flip = SDL_FLIP_NONE, double angle = 0.0, SDL_Point *center = NULL );
		//Renders sprite from sheet at given point
		void renderSprite( int x, int y, int sheetColumn, int sheetRow, SDL_RendererFlip flip = SDL_FLIP_NONE, double angle = 0.0, SDL_Point *center = NULL );

		//Get image size
		int getWidth();
		int getHeight();
	protected:
	private:
		//The actual hardware texture
		SDL_Texture *texture;

		//Texture size
		int sheetWidth;
		int sheetHeight;
		//Sprite size
		int width;
		int height;
};

#endif // GAMETEXTURE_H
