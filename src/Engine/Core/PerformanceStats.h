#pragma once
#ifndef PERFORMANCESTATS_H_
#define PERFORMANCESTATS_H_

#include <iostream>
#include <vector>
#include <map>

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

	const static int avgFrameRateCount = 1440;
	std::vector<int> framerateList;
	static int currentFrameCount;

	static std::vector< std::pair<std::string, Uint32>> m_mSystemUpdateTimes;
	static std::vector<std::pair<std::string, Uint32>> m_mSystemRenderTimes;

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