#pragma once

#include <Engine\Core\DebugUI.hpp>
#include <Engine\Core\PerformanceStats.h>

#include <deque>

struct FrameTimeStat
{
	std::string m_sName;
	std::deque<double> m_qTimes;

	double m_frameTimeArrayX[100] = { 0 };
	double m_frameTimeArray[100] = { 0 };
	double m_fMaxFrameTime = 0;
	double m_fAvgFrameTime = 0;

	double* m_targetValue;

	FrameTimeStat(std::string name, double* targetValue)
	{
		m_sName = name;
		m_targetValue = targetValue;
	}
};

class Profiler : public DebugUI
{
private:

	PerformanceStats* m_pStats;
	std::deque<float> m_qFrameRates;
	std::vector<FrameTimeStat> m_vFrameTimeStats;

	const int m_iMaxSamples = 100;
	const double m_dMaxFPS = 200;

public:
	void InitializeInterface() override;
	void DoInterface() override;
};