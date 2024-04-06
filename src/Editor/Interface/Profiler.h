#pragma once

#include <Engine\Core\ToolUI.hpp>
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

	FrameTimeStat(std::string name)
	{
		m_sName = name;
	}
};

class Profiler : public EditorToolUI
{
private:

	PerformanceStats* m_pStats = nullptr;
	std::deque<float> m_qFrameRates;
	std::vector<FrameTimeStat> m_vFrameTimeStats;

	const int m_iMaxSamples = 100;
	const double m_dMaxFPS = 200;

public:
	void InitializeInterface() override;
	void DoInterface() override;
};