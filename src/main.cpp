#include "BGE_Engine.h"

#include <iostream>

int main() {	//In order to build for Windows, change to WinMain()
	BGE_Engine game;

	if ( !game.init() ) {
		printf( "Failed to initialise SDL.\n" );
	}
	else {
		if ( !game.load() ) {
			printf( "Failed to load media.\n" );
		} else {
			game.start();
		}
	}

	game.close();

	return 0;
}
