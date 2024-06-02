#pragma once
#ifndef PERFORMANCESTATS_H_
#define PERFORMANCESTATS_H_

#include <iostream>
#include <vector>
#include <map>
#include <chrono>

#define NOMINMAX
#include <Windows.h>

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

	double GetPerformanceMeasurementInMicroSeconds()
	{
		int returnVal = m_measurementTime.count();
		return returnVal;
	}
};

/**
*Stores engine performance stats.
*/
struct PerformanceStats
{
	friend class Application;

private:

	const int m_iAvgFrameRateCount = 1440;
	std::vector<int> m_vFramerateList;
	int m_iCurrentFrameCount = 0;

	std::map<std::string, PerformanceMeasurement> m_mSystemUpdateTimes;
	std::map<std::string, PerformanceMeasurement> m_mSystemRenderTimes;

	std::map<std::string, PerformanceMeasurement> m_mPerformanceMeasurements;

	/**
	*Updates the performance stats. Done every frame by the engine.
	*/
	void UpdatePerformanceStats();
	void UpdateMemoryUsageStats();

	DWORDLONG GetTotalVirtualMemory();
	DWORDLONG GetVirtualMemoryUsed();
	SIZE_T GetVirtualMemoryUsedByEngine();
	DWORDLONG GetVirtualMemoryAvailable();

	DWORDLONG GetTotalPhysicalMemory();
	DWORDLONG GetPhysicalMemoryUsed();
	SIZE_T GetPhysicalMemoryUsedByEngine();
	DWORDLONG GetPhysicalMemoryAvailable();

public:

	bool m_bLogPerformance;

	static double m_dDeltaT;
	double m_dFPS = 0;
	double m_dAvgFPS;

	PerformanceStats();

	float m_fTotalVirtualMemory;
	float m_fVirtualMemoryUsage;
	float m_fVirtualMemoryUsageByEngine;
	float m_fVirtualMemoryAvailable;

	float m_fTotalPhysicalMemory;
	float m_fPhysicalMemoryUsage;
	float m_fPhysicalMemoryUsageByEngine;
	float m_fPhysicalMemoryAvailable;

	float m_fOtherMemoryUsage;
	float m_fResourceMemoryUsage;
	float m_fEntityMemoryUsage;

	/**
	*Logs perfomance information for the current frame.
	*/
	void LogPerformanceStats();

	void ClearComponentMeasurements();

	std::map<std::string, PerformanceMeasurement>* GetSystemUpdateTimes() { return &m_mSystemUpdateTimes; }
	std::map<std::string, PerformanceMeasurement>* GetSystemRenderTimes() { return &m_mSystemRenderTimes; }

	void AddPerformanceMeasurement(std::string name);
	void StartPerformanceMeasurement(std::string name);
	void EndPerformanceMeasurement(std::string name);

	void AddComponentMeasurement(std::string name);
	void StartComponentMeasurement(std::string name, bool update);
	void EndComponentMeasurement(std::string name, bool update);

	PerformanceMeasurement* GetComponentUpdateMeasurement(std::string name);
	PerformanceMeasurement* GetComponentRenderMeasurement(std::string name);
	PerformanceMeasurement* GetPerformanceMeasurement(std::string name);
};

#endif