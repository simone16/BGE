#ifndef GAMETIMER_H
#define GAMETIMER_H

#include <SDL2/SDL.h>
#include <stdint.h>

class BGE_Timer {
	public:
		BGE_Timer();
		virtual ~BGE_Timer();

		void start();
		void pause();
		void resume();
		void stop();

		uint32_t getMs();
		bool isStarted();
		bool isPaused();
	protected:
	private:
		uint32_t startTime;
		uint32_t pauseTime;

		bool started;
		bool paused;
};

#endif // GAMETIMER_H
