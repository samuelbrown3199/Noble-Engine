#include "PerformanceStats.h"

#include "Application.h"

namespace NobleCore
{
	double PerformanceStats::deltaT = 0;
	double PerformanceStats::avgFPS = 0;
	double PerformanceStats::frameTime = 0;
	double PerformanceStats::preUpdateTime = 0;
	double PerformanceStats::updateTime = 0;
	double PerformanceStats::renderTime = 0;
	double PerformanceStats::cleanupTime = 0;

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
		std::cout << "AVG FPS: " << avgFPS << " | Frame Time: " << frameTime << " | Pre Update Time: " << preUpdateTime <<" | Update Time: " << updateTime << " | Render Time: " << renderTime << " | Cleanup Time: " << cleanupTime << std::endl;
	}
}