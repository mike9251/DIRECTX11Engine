#include "Timer.h"

Timer::Timer()
{
	this->start = std::chrono::high_resolution_clock::now();
	this->stop = std::chrono::high_resolution_clock::now();
}

double Timer::GetMillisecondsElapsed()
{
	if (this->isRunning)
	{
		auto elapsed = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - this->start);
		return elapsed.count();
	}
	else
	{
		auto elapsed = std::chrono::duration<double, std::milli>(this->stop - this->start);
		return elapsed.count();
	}
}

bool Timer::Restart()
{
	this->start = std::chrono::high_resolution_clock::now();
	this->isRunning = true;
	return true;
}

bool Timer::Stop()
{
	if (this->isRunning)
	{
		this->isRunning = false;
		this->stop = std::chrono::high_resolution_clock::now();
		return true;
	}
	else
	{
		return false;
	}
}

bool Timer::Start()
{
	if (this->isRunning)
	{
		return false;
	}
	else
	{
		this->isRunning = true;
		this->start = std::chrono::high_resolution_clock::now();
		return true;
	}
}
