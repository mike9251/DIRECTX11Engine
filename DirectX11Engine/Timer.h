#pragma once
#include <chrono>

class Timer
{
public:
	Timer();
	double GetMillisecondsElapsed();
	bool Restart();
	bool Stop();
	bool Start();

private:
	bool isRunning = false;

	std::chrono::time_point<std::chrono::steady_clock> start;
	std::chrono::time_point<std::chrono::steady_clock> stop;
};