#include "BGE_Engine.h"

#include <iostream>

int main(int argc, char *argv[]) {	//In order to build for Windows, change to WinMain()
	BGE_Engine game;

	if ( !game.init() ) {
		printf( "Failed to initialise SDL.\n" );
	}
	else {
		if ( !game.load() ) {
			printf( "Failed to load media.\n" );
		} else {
			if (argc == 1) {
				game.newWorld();
			}
			else {
				std::string filename (argv[1]);
				game.loadFile( filename);
			}
			game.start();
		}
	}

	game.close();

	return 0;
}
