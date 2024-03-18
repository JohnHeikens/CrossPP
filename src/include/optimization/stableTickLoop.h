#pragma once
#include "math/timemath.h"
#include <thread>
struct stableLoop {
	microseconds microSecondsPerIteration = 0;
	stableLoop(microseconds microSecondsPerIteration) : microSecondsPerIteration(microSecondsPerIteration) {}
	microseconds nextMicroSeconds = 0;
	inline void waitTillNextLoopTime() {
		cmicroseconds& currentMicroSeconds = getmicroseconds();
		nextMicroSeconds = nextMicroSeconds ? nextMicroSeconds : currentMicroSeconds;
		cmicroseconds& timeRemaining = nextMicroSeconds - currentMicroSeconds;
		if (timeRemaining > 0)
		{
			std::this_thread::sleep_for(std::chrono::microseconds(timeRemaining));
			//miliseconds milisecondsToSleep = getMiliseconds(timeRemaining) - 1;
			//if (milisecondsToSleep > 0)
			//{
			//	Sleep((DWORD)milisecondsToSleep);
			//}
			//while (getmicroseconds() < timeRemaining) {};
			nextMicroSeconds += microSecondsPerIteration;
		}
		else//lag
		{
			nextMicroSeconds += (((currentMicroSeconds - nextMicroSeconds) / microSecondsPerIteration) + 1) * microSecondsPerIteration;
			/*
			optimizing:
			while (nextFrameTime < currentMicroSeconds)
			{
				nextFrameTime += frameTime;
			}
			*/
			// 1 + 10 + 10 ... < 25? 1 + (25 - 1) / 10 * 10
			// + 1 to make it from last frame time to next frame time
			//nextFrameTime += math::floor(currentMicroSeconds - nextFrameTime, frameTime) + frameTime;
		}
	}
};