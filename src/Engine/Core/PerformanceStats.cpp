#include "PerformanceStats.h"
#include "Application.h"
#include "../Useful.h"


double PerformanceStats::deltaT = 0;
double PerformanceStats::avgFPS = 0;
double PerformanceStats::frameTime = 0;
double PerformanceStats::preUpdateTime = 0;
double PerformanceStats::updateTime = 0;
double PerformanceStats::renderTime = 0;
double PerformanceStats::cleanupTime = 0;
bool PerformanceStats::printPerformance = false;

std::vector< std::pair<std::string, Uint32>> PerformanceStats::m_mSystemUpdateTimes;
std::vector< std::pair<std::string, Uint32>> PerformanceStats::m_mSystemRenderTimes;

void PerformanceStats::ResetPerformanceStats()
{
	frameStart = SDL_GetTicks();
	preUpdateStart = 0;
	updateTime = 0;
	renderTime = 0;
	cleanupTime = 0;
}

void PerformanceStats::UpdatePerformanceStats()
{
	frameTime = SDL_GetTicks() - frameStart;
	fps = 1000.0f / frameTime;
	deltaT = 1.0f / fps;
	framerateList.push_back(fps);
	currentFrameCount++;

	if (avgFrameRateCount == currentFrameCount)
	{
		avgFPS = 0;
		for (int i = 0; i < framerateList.size(); i++)
		{
			avgFPS += framerateList.at(i);
		}
		framerateList.clear();
		avgFPS /= avgFrameRateCount;
		currentFrameCount = 0;
	}
}

void PerformanceStats::PrintOutPerformanceStats()
{
	if (!printPerformance)
		return;

	std::string performanceStatsString = FormatString("AVG FPS: %2f | Frame Time: %2f | Pre Update Time: %2f | Update Time: %2f | Render Time: %2f | Cleanup Time: %2f", 
		avgFPS, frameTime, preUpdateTime, updateTime, renderTime, cleanupTime);

	for (int i = 0; i < m_mSystemUpdateTimes.size(); i++)
	{
		std::string sysInfo = FormatString("%s Update Time: %2f | Render Time: %2f", m_mSystemUpdateTimes.at(i).first, m_mSystemUpdateTimes.at(i).second, m_mSystemRenderTimes.at(i).second);
		std::cout << sysInfo << std::endl;
	}

	std::cout << performanceStatsString << std::endl;
}