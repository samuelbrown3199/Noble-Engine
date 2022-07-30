#pragma once
#ifndef PERFORMANCESTATS_H_
#define PERFORMANCESTATS_H_

#include <iostream>
#include <vector>

#include <SDL/SDL.h>

/**
*Stores engine performance stats.
*/
struct PerformanceStats
{
	friend class Application;

private:
	Uint32 frameStart, preUpdateStart, renderStart, updateStart, cleanupStart;
	double fps = 0;

	const int avgFrameRateCount = 60;
	std::vector<int> framerateList;
	int currentFrameCount = 0;

	/**
	*Resets the performance stats. Done every frame by the engine.
	*/
	void ResetPerformanceStats();
	/**
	*Updates the performance stats. Done every frame by the engine.
	*/
	void UpdatePerformanceStats();

public:

	static bool printPerformance;

	static double deltaT;

	static double avgFPS;
	static double frameTime;
	static double preUpdateTime;
	static double updateTime;
	static double renderTime;
	static double cleanupTime;

	/**
	*Prints out perfomance information for the current frame.
	*/
	static void PrintOutPerformanceStats();
};

#endif