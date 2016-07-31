#include "BGE_Texture.h"

#include "BGE_Engine.h"

//#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
//#include <string>

BGE_Engine *BGE_Texture::engine;

BGE_Texture::BGE_Texture() {
	//Initialize
	texture = NULL;
	sheetWidth = 0;
	sheetHeight = 0;
	width = 0;
	height = 0;
	offsetX = 0;
    offsetY = 0;
}

BGE_Texture::~BGE_Texture() {
	//Deallocate
	free();
}

bool BGE_Texture::loadFromFile( std::string path, int keyRed, int keyGreen, int keyBlue ) {
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture *newTexture = NULL;

	//Load image at specified path
	SDL_Surface *loadedSurface = IMG_Load( path.c_str() );

	if( loadedSurface == NULL ) {
		printf( "Unable to load image %s!\nSDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else {
		//Color key image
		if ( keyRed >= 0 && keyGreen >= 0 && keyBlue >= 0 ) {
			SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, keyRed, keyGreen, keyBlue ) );
		}

		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface( BGE_Texture::engine->getRenderer(), loadedSurface );

		if( newTexture == NULL ) {
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}
		else {
			//Store image size
			sheetWidth = loadedSurface->w;
			sheetHeight = loadedSurface->h;
			width = loadedSurface->w;
			height = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	texture = newTexture;

	//Return success
	return texture != NULL;
}

void BGE_Texture::setSpriteSize(int nwidth, int nheight) {
	width = nwidth;
	height = nheight;
	setSpriteOffset(0,0);
}

void BGE_Texture::setSpriteOffset(int x, int y) {
    offsetX = x - width / 2;
    offsetY = y - height / 2;
}

BGE_2DVect BGE_Texture::getSpriteOffset() {
	BGE_2DVect offset (offsetX, offsetY);
	return offset;
}

bool BGE_Texture::loadFromRenderedText( std::string textureText, Uint8 red, Uint8 green, Uint8 blue ) {
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Color textColor = {red, green, blue};
	SDL_Surface *textSurface = TTF_RenderText_Blended( BGE_Texture::engine->getFont(), textureText.c_str(), textColor );
	// All options are:
	//TTF_RenderText_Solid()    Fastest, only transparent and foreground colors
	//TTF_RenderText_Shaded()   Medium, shades but background is not transparent
	//TTF_RenderText_Blended()  Slowest, shades + transparency

	if( textSurface != NULL ) {
		//Create texture from surface pixels
		texture = SDL_CreateTextureFromSurface( BGE_Texture::engine->getRenderer(), textSurface );

		if( texture == NULL ) {
			printf( "Unable to create texture from rendered text \"%s\"!\nSDL Error: %s\n", textureText.c_str(), SDL_GetError() );
		}
		else {
			//Get image size
			sheetWidth = textSurface->w;
			sheetHeight = textSurface->h;
			width = textSurface->w;
			height = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface( textSurface );
	}
	else {
		printf( "Unable to create texture from text \"%s\"!\nSDL Error: %s\n", textureText.c_str(), TTF_GetError() );
	}

	//Return success
	return texture != NULL;
}

void BGE_Texture::free() {
	//Free texture if it exists
	if( texture != NULL ) {
		SDL_DestroyTexture( texture );
		texture = NULL;
		sheetWidth = 0;
		sheetHeight = 0;
	}
}

void BGE_Texture::setColor( Uint8 red, Uint8 green, Uint8 blue ) {
	//Modulate texture rgb
	SDL_SetTextureColorMod( texture, red, green, blue );
}

void BGE_Texture::setBlendMode( SDL_BlendMode blending ) {
	//Set blending function
	SDL_SetTextureBlendMode( texture, blending );
}

void BGE_Texture::setAlpha( Uint8 alpha ) {
	//Modulate texture alpha
	SDL_SetTextureAlphaMod( texture, alpha );
}

void BGE_Texture::render( float x, float y, SDL_RendererFlip flip, double angle, SDL_Point *center ) {
	//Apply viewport offset.
    BGE_2DVect position = -(engine->getViewportOffset());
    position.x += x-sheetWidth/2;
    position.y += y-sheetHeight/2;
	//Rendering area on screen.
	SDL_Rect screenClip = {int(position.x), int(position.y), sheetWidth, sheetHeight };
	//Render to screen
	SDL_RenderCopyEx( BGE_Texture::engine->getRenderer(), texture, NULL, &screenClip, angle, center, flip );
}

void BGE_Texture::renderSprite( float x, float y, int sheetColumn, int sheetRow, SDL_RendererFlip flip, double angle, SDL_Point *center ) {
	//Apply viewport offset.
	BGE_2DVect position = -(engine->getViewportOffset());
	position.x += x+offsetX;
    position.y += y+offsetY;
	//Rendering area on screen.
	SDL_Rect screenClip = {int(position.x), int(position.y), width, height };
	//Area to blit from in the sheet.
    SDL_Rect sheetClip = {sheetColumn * width, sheetRow * height, width, height};
	//Render to screen
	SDL_RenderCopyEx( BGE_Texture::engine->getRenderer(), texture, &sheetClip, &screenClip, angle, center, flip );
}

int BGE_Texture::getWidth() {
	return sheetWidth;
}

int BGE_Texture::getHeight() {
	return sheetHeight;
}
