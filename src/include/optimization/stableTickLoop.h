#pragma once
#include "math/timemath.h"
#include <thread>
struct stableLoop {
	std::chrono::microseconds microSecondsPerIteration = std::chrono::microseconds();
	//caution! we are calling std::chrono::duration(int64_t) here
	stableLoop(microseconds microSecondsPerIteration) : microSecondsPerIteration(microSecondsPerIteration) {}
	std::chrono::steady_clock::time_point nextLoopTime = std::chrono::steady_clock::now();
	inline void waitTillNextLoopTime() {
		std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
		//cmicroseconds& currentMicroSeconds = getmicroseconds();
		//nextMicroSeconds = nextMicroSeconds ? nextMicroSeconds : currentMicroSeconds;
		//cmicroseconds& timeRemaining = nextMicroSeconds - currentMicroSeconds;
		if (nextLoopTime > currentTime)
		{
			std::this_thread::sleep_until(nextLoopTime);
			//miliseconds milisecondsToSleep = getMiliseconds(timeRemaining) - 1;
			//if (milisecondsToSleep > 0)
			//{
			//	Sleep((DWORD)milisecondsToSleep);
			//}
			//while (getmicroseconds() < timeRemaining) {};
			nextLoopTime += microSecondsPerIteration;
			//nextMicroSeconds += microSecondsPerIteration;
		}
		else//lag
		{
			nextLoopTime += (((currentTime - nextLoopTime) / microSecondsPerIteration) + 1) * microSecondsPerIteration;
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