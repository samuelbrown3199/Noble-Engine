#pragma once
#ifndef PERFORMANCESTATS_H_
#define PERFORMANCESTATS_H_

#include <iostream>
#include <vector>
#include <map>
#include <chrono>

#include <SDL/SDL.h>

struct PerformanceMeasurement
{
	std::chrono::time_point<std::chrono::high_resolution_clock> m_measurementStart;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_measurementEnd;
	std::chrono::duration<double, std::micro> m_measurementTime;

	void StartMeasurement()
	{
		m_measurementStart = std::chrono::high_resolution_clock::now();
	}

	void EndMeasurement()
	{
		m_measurementEnd = std::chrono::high_resolution_clock::now();
		m_measurementTime = std::chrono::duration_cast<std::chrono::microseconds>(m_measurementEnd - m_measurementStart);
	}
};

/**
*Stores engine performance stats.
*/
struct PerformanceStats
{
	friend class Application;

private:

	const int avgFrameRateCount = 1440;
	std::vector<int> framerateList;
	int currentFrameCount = 0;

	std::vector<std::pair<std::string, Uint32>> m_mSystemUpdateTimes;
	std::vector<std::pair<std::string, Uint32>> m_mSystemRenderTimes;

	std::vector < std::pair<std::string, PerformanceMeasurement>> m_mPerformanceMeasurements;

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

	PerformanceStats();

	/**
	*Logs perfomance information for the current frame.
	*/
	void LogPerformanceStats();

	std::vector<std::pair<std::string, Uint32>> GetSystemUpdateTimes() { return m_mSystemUpdateTimes; }
	std::vector<std::pair<std::string, Uint32>> GetSystemRenderTimes() { return m_mSystemRenderTimes; }

	void AddPerformanceMeasurement(std::string name);
	void StartPerformanceMeasurement(std::string name);
	void EndPerformanceMeasurement(std::string name);
	double GetPerformanceMeasurementInMicroSeconds(std::string name);
};

#endif