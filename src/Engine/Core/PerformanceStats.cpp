#include "PerformanceStats.h"
#include "Application.h"
#include "../Useful.h"

double PerformanceStats::m_dDeltaT;

void PerformanceStats::ResetPerformanceStats()
{
	frameStart = SDL_GetTicks();
	preUpdateStart = 0;
	m_dPreviousRenderTime = m_frameTimes[3];
	m_dPreviousFrameTime = m_frameTimes[0];

	for (int i = 0; i < 5; i++)
	{
		m_frameTimes[i] = 0;
	}
}

void PerformanceStats::UpdatePerformanceStats()
{
	m_frameTimes[0] = SDL_GetTicks() - frameStart;
	m_dFPS = 1000.0f / m_frameTimes[0];
	m_dDeltaT = 1.0f / m_dFPS;
	framerateList.push_back(m_dFPS);
	currentFrameCount++;

	if (avgFrameRateCount == currentFrameCount)
	{
		m_dAvgFPS = 0;
		for (int i = 0; i < framerateList.size(); i++)
		{
			m_dAvgFPS += framerateList.at(i);
		}
		framerateList.clear();
		m_dAvgFPS /= avgFrameRateCount;
		currentFrameCount = 0;
	}
}

void PerformanceStats::PrintOutPerformanceStats()
{
	if (!m_bPrintPerformance)
		return;

	if (currentFrameCount != 0)
		return;

	std::string performanceStatsString = FormatString("AVG FPS: %2f | Frame Time: %2f | Pre Update Time: %2f | Update Time: %2f | Render Time: %2f | Cleanup Time: %2f\n", 
		m_dAvgFPS, m_frameTimes[0], m_frameTimes[1], m_frameTimes[2], m_frameTimes[3], m_frameTimes[4]);

	for (int i = 0; i < m_mSystemUpdateTimes.size(); i++)
	{
		performanceStatsString += FormatString("%s Update Time: %2f | Render Time: %2f\n", m_mSystemUpdateTimes.at(i).first, m_mSystemUpdateTimes.at(i).second, m_mSystemRenderTimes.at(i).second);
	}

	Logger::LogInformation(performanceStatsString);
}