#pragma once

#include <Engine\Core\ToolUI.h>
#include <Engine\Core\PerformanceStats.h>

#include <deque>


namespace NobleProfiling
{
	const int m_iMaxSamples = 500;
};

struct FrameTimeStat
{
	std::string m_sName;
	std::deque<double> m_qTimes;

	double m_frameTimeArrayX[NobleProfiling::m_iMaxSamples] = { 0 };
	double m_frameTimeArray[NobleProfiling::m_iMaxSamples] = { 0 };
	double m_fMaxFrameTime = 0;
	double m_fAvgFrameTime = 0;

	FrameTimeStat(std::string name)
	{
		m_sName = name;
	}
};

struct MemoryStat
{
	std::string m_sName;
	std::deque<double> m_qMemoryValues;
	float* m_fTargetValue;

	double m_dMemoryValuesX[NobleProfiling::m_iMaxSamples] = { 0 };
	double m_dMemoryValues[NobleProfiling::m_iMaxSamples] = { 0 };

	MemoryStat(std::string name, float* targetValue)
	{
		m_sName = name;
		m_fTargetValue = targetValue;
	}
};

class Profiler : public EditorToolUI
{

private:

	PerformanceStats* m_pStats = nullptr;
	std::deque<float> m_qFrameRates;
	std::vector<FrameTimeStat> m_vFrameTimeStats;
	
	std::vector<FrameTimeStat> m_vComponentUpdateTimes;
	std::vector<FrameTimeStat> m_vComponentRenderTimes;

	std::vector<MemoryStat> m_vMemoryStats;

	const double m_dMaxFPS = 200;

	float m_fFrameRateArray[NobleProfiling::m_iMaxSamples] = { 0 };
	float m_fMaxFrameTime = 0;
	float m_fMaxFrameRate = 0;
	float m_fAvgFrameRate = 0;

	float m_fMaxMemory = 0;

	void ProcessData();

public:

	void InitializeInterface(ImGuiWindowFlags defaultFlags) override;
	void DoInterface() override;

	void AddComponentMeasurement(std::string name);
};