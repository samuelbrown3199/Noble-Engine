#include "PerformanceStats.h"
#include "Application.h"
#include "../Useful.h"

double PerformanceStats::m_dDeltaT;

void PerformanceStats::UpdatePerformanceStats()
{
	double frameMilliseconds = GetPerformanceMeasurementInMicroSeconds("Frame") / 1000;
	m_dFPS = 1000.0f / frameMilliseconds;
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

PerformanceStats::PerformanceStats()
{
	AddPerformanceMeasurement("Frame");
	AddPerformanceMeasurement("Pre-Update");
	AddPerformanceMeasurement("Update");
	AddPerformanceMeasurement("Render");
	AddPerformanceMeasurement("Cleanup");

	currentFrameCount = 0;
}

void PerformanceStats::LogPerformanceStats()
{
	if (!m_bLogPerformance)
		return;

	if (currentFrameCount != 0)
		return;

	std::string performanceStatsString = FormatString("FPS: %.2f | Delta Time: %.5f", m_dFPS, m_dDeltaT);
	for (int i = 0; i < m_mPerformanceMeasurements.size(); i++)
	{
		if (i != m_mPerformanceMeasurements.size())
			performanceStatsString += " | ";

		performanceStatsString += FormatString("%s: %.2f", m_mPerformanceMeasurements.at(i).first, GetPerformanceMeasurementInMicroSeconds(m_mPerformanceMeasurements.at(i).first) / 1000);
	}
	performanceStatsString += "\n";
	for (int i = 0; i < m_mSystemUpdateTimes.size(); i++)
	{
		performanceStatsString += FormatString("%s Update Time: %.2f | Render Time: %.2f\n", m_mSystemUpdateTimes.at(i).first, m_mSystemUpdateTimes.at(i).second, m_mSystemRenderTimes.at(i).second);
	}

	Logger::LogInformation(performanceStatsString);
}

void PerformanceStats::AddPerformanceMeasurement(std::string name)
{
	for (int i = 0; i < m_mPerformanceMeasurements.size(); i++)
	{
		if (m_mPerformanceMeasurements.at(i).first == name)
			Logger::LogError("Tried to add duplicate performance measurement.", 2);
	}

	m_mPerformanceMeasurements.push_back(std::make_pair(name, PerformanceMeasurement()));
}

void PerformanceStats::StartPerformanceMeasurement(std::string name)
{
	for (int i = 0; i < m_mPerformanceMeasurements.size(); i++)
	{
		if (m_mPerformanceMeasurements.at(i).first == name)
			m_mPerformanceMeasurements.at(i).second.StartMeasurement();
	}
}

void PerformanceStats::EndPerformanceMeasurement(std::string name)
{
	for (int i = 0; i < m_mPerformanceMeasurements.size(); i++)
	{
		if (m_mPerformanceMeasurements.at(i).first == name)
			m_mPerformanceMeasurements.at(i).second.EndMeasurement();
	}
}

double PerformanceStats::GetPerformanceMeasurementInMicroSeconds(std::string name)
{
	for (int i = 0; i < m_mPerformanceMeasurements.size(); i++)
	{
		if (m_mPerformanceMeasurements.at(i).first == name)
		{
			int returnVal = m_mPerformanceMeasurements.at(i).second.m_measurementTime.count();
			return returnVal;
		}
	}
}