#include "BGE_Timer.h"

BGE_Timer::BGE_Timer() {
	startTime = 0;
	pauseTime = 0;

	started = false;
	paused = false;
}

BGE_Timer::~BGE_Timer() {
	//dtor
}

void BGE_Timer::start() {
	//Start the timer
	started = true;

	//Unpause the timer
	paused = false;

	//Get the current clock time
	startTime = SDL_GetTicks();
	pauseTime = 0;
}

void BGE_Timer::pause() {
	//If the timer is running and isn't already paused
	if( started && !paused ) {
			//Pause the timer
			paused = true;

			//Calculate the paused ticks
			pauseTime = SDL_GetTicks() - startTime;
			startTime = 0;
		}
}

void BGE_Timer::resume() {
	//If the timer is running and paused
	if( started && paused ) {
			//Unpause the timer
			paused = false;

			//Reset the starting ticks
			startTime = SDL_GetTicks() - pauseTime;

			//Reset the paused ticks
			pauseTime = 0;
		}
}

void BGE_Timer::stop() {
	//Stop the timer
	started = false;

	//Unpause the timer
	paused = false;

	//Clear tick variables
	startTime = 0;
	pauseTime = 0;
}

Uint32 BGE_Timer::getMs() {
	//The actual timer time
	uint32_t time = 0;

	//If the timer is running
	if( started ) {
			//If the timer is paused
			if( paused ) {
					//Return the number of ticks when the timer was paused
					time = pauseTime;
				}
			else {
					//Return the current time minus the start time
					time = SDL_GetTicks() - startTime;
				}
		}

	return time;
}

bool BGE_Timer::isStarted() {
	//Timer is running and paused or unpaused
	return started;
}

bool BGE_Timer::isPaused() {
	//Timer is running and paused
	return paused && started;
}
