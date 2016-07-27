#include "BGE_Engine.h"

int main() {
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
