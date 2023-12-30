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

	const int avgFrameRateCount = 1440;
	std::vector<int> framerateList;
	int currentFrameCount;

	std::vector<std::pair<std::string, Uint32>> m_mSystemUpdateTimes;
	std::vector<std::pair<std::string, Uint32>> m_mSystemRenderTimes;

	/**
	*Resets the performance stats. Done every frame by the engine.
	*/
	void ResetPerformanceStats();
	/**
	*Updates the performance stats. Done every frame by the engine.
	*/
	void UpdatePerformanceStats();

public:

	bool m_bLogPerformance;

	static double m_dDeltaT;
	double m_dFPS = 0;
	double m_dAvgFPS;
	double m_dPreviousRenderTime;
	double m_dPreviousFrameTime;
	double m_frameTimes[5];

	/**
	*Logs perfomance information for the current frame.
	*/
	void LogPerformanceStats();

	std::vector<std::pair<std::string, Uint32>> GetSystemUpdateTimes() { return m_mSystemUpdateTimes; }
	std::vector<std::pair<std::string, Uint32>> GetSystemRenderTimes() { return m_mSystemRenderTimes; }
};

#endif