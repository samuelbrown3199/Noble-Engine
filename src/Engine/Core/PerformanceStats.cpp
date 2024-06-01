#include "PerformanceStats.h"
#include "Application.h"
#include "../Useful.h"

#include "psapi.h"

double PerformanceStats::m_dDeltaT;

void PerformanceStats::UpdatePerformanceStats()
{
	double frameMilliseconds = GetPerformanceMeasurementInMicroSeconds("Frame") / 1000;
	m_dFPS = 1000.0f / frameMilliseconds;
	m_dDeltaT = 1.0f / m_dFPS;
	m_vFramerateList.push_back(m_dFPS);
	m_iCurrentFrameCount++;

	if (m_iAvgFrameRateCount == m_iCurrentFrameCount)
	{
		m_dAvgFPS = 0;
		for (int i = 0; i < m_vFramerateList.size(); i++)
		{
			m_dAvgFPS += m_vFramerateList.at(i);
		}
		m_vFramerateList.clear();
		m_dAvgFPS /= m_iAvgFrameRateCount;
		m_iCurrentFrameCount = 0;
	}
}

PerformanceStats::PerformanceStats()
{
	AddPerformanceMeasurement("Frame");
	AddPerformanceMeasurement("Pre-Update");
	AddPerformanceMeasurement("Update");
	AddPerformanceMeasurement("Render");
	AddPerformanceMeasurement("EditorUpdate");
	AddPerformanceMeasurement("EditorRender");
	AddPerformanceMeasurement("Cleanup");

	m_mSystemUpdateTimes = std::vector<std::pair<std::string, PerformanceMeasurement>>();
	m_mSystemRenderTimes = std::vector<std::pair<std::string, PerformanceMeasurement>>();

	m_iCurrentFrameCount = 0;
}

void PerformanceStats::LogPerformanceStats()
{
	if (!m_bLogPerformance)
		return;

	if (m_iCurrentFrameCount != 0)
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
		performanceStatsString += FormatString("%s Update Time: %.2f | Render Time: %.2f\n", m_mSystemUpdateTimes.at(i).first, m_mSystemUpdateTimes.at(i).second.m_measurementTime.count()/1000, m_mSystemRenderTimes.at(i).second.m_measurementTime.count() / 1000);
	}

	LogInfo(performanceStatsString);
}

void PerformanceStats::ClearComponentMeasurements()
{
	m_mSystemUpdateTimes.clear();
	m_mSystemRenderTimes.clear();
}

void PerformanceStats::AddPerformanceMeasurement(std::string name)
{
	for (int i = 0; i < m_mPerformanceMeasurements.size(); i++)
	{
		if (m_mPerformanceMeasurements.at(i).first == name)
			LogFatalError("Tried to add duplicate performance measurement.");
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

void PerformanceStats::AddComponentMeasurement(std::string name)
{
	for (int i = 0; i < m_mSystemUpdateTimes.size(); i++)
	{
		if (m_mSystemUpdateTimes.at(i).first == name)
			LogFatalError("Tried to add duplicate performance measurement.");
	}

	m_mSystemUpdateTimes.push_back(std::make_pair(name, PerformanceMeasurement()));
	m_mSystemRenderTimes.push_back(std::make_pair(name, PerformanceMeasurement()));
}

void PerformanceStats::StartComponentMeasurement(std::string name, bool update)
{
	std::vector<std::pair<std::string, PerformanceMeasurement>>* targetList = &m_mSystemUpdateTimes;
	if (!update)
		targetList = &m_mSystemRenderTimes;

	for (int i = 0; i < targetList->size(); i++)
	{
		if (targetList->at(i).first == name)
			targetList->at(i).second.StartMeasurement();
	}
}

void PerformanceStats::EndComponentMeasurement(std::string name, bool update)
{
	std::vector<std::pair<std::string, PerformanceMeasurement>>* targetList = &m_mSystemUpdateTimes;
	if (!update)
		targetList = &m_mSystemRenderTimes;

	for (int i = 0; i < targetList->size(); i++)
	{
		if (targetList->at(i).first == name)
			targetList->at(i).second.EndMeasurement();
	}
}

void PerformanceStats::UpdateMemoryUsageStats()
{
	if (m_iAvgFrameRateCount < m_iCurrentFrameCount)
		return;

	m_fPhysicalMemoryUsageByEngine = (GetPhysicalMemoryUsedByEngine() / 1024.0f) / 1024.0f;
	m_fVirtualMemoryUsageByEngine = (GetVirtualMemoryUsedByEngine() / 1024.0f) / 1024.0f;

	m_fPhysicalMemoryUsage = (GetPhysicalMemoryUsed() / 1024.0f) / 1024.0f;
	m_fVirtualMemoryUsage = (GetVirtualMemoryUsed() / 1024.0f) / 1024.0f;

	m_fTotalPhysicalMemory = (GetTotalPhysicalMemory() / 1024.0f) / 1024.0f;
	m_fTotalVirtualMemory = (GetTotalVirtualMemory() / 1024.0f) / 1024.0f;

	m_fPhysicalMemoryAvailable = (GetPhysicalMemoryAvailable() / 1024.0f) / 1024.0f;
	m_fVirtualMemoryAvailable = (GetVirtualMemoryAvailable() / 1024.0f) / 1024.0f;
}

DWORDLONG PerformanceStats::GetTotalVirtualMemory()
{
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    if(!GlobalMemoryStatusEx(&memInfo))
	{
		LogFatalError("Failed to get memory status.");
		return 0;
	}
    DWORDLONG totalVirtualMem = memInfo.ullTotalPageFile;

    return totalVirtualMem;
}

DWORDLONG PerformanceStats::GetVirtualMemoryUsed()
{
    MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	if (!GlobalMemoryStatusEx(&memInfo))
	{
		LogFatalError("Failed to get memory status.");
		return 0;
	}
	DWORDLONG virtualMemUsed = memInfo.ullTotalPageFile - memInfo.ullAvailPageFile;

	return virtualMemUsed;
}

SIZE_T PerformanceStats::GetVirtualMemoryUsedByEngine()
{
    PROCESS_MEMORY_COUNTERS_EX pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
    SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;

    return virtualMemUsedByMe;
}

SIZE_T PerformanceStats::GetVirtualMemoryAvailable()
{
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	if (!GlobalMemoryStatusEx(&memInfo))
	{
		LogFatalError("Failed to get memory status.");
		return 0;
	}
	SIZE_T virtualMemAvailable = memInfo.ullAvailPageFile;

	return virtualMemAvailable;
}

DWORDLONG PerformanceStats::GetTotalPhysicalMemory()
{
    MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	if (!GlobalMemoryStatusEx(&memInfo))
	{
		LogFatalError("Failed to get memory status.");
		return 0;
	}
	DWORDLONG totalPhysMem = memInfo.ullTotalPhys;

	return totalPhysMem;
}

DWORDLONG PerformanceStats::GetPhysicalMemoryUsed()
{
    MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	if (!GlobalMemoryStatusEx(&memInfo))
	{
		LogFatalError("Failed to get memory status.");
		return 0;
	}
	DWORDLONG physMemUsed = memInfo.ullTotalPhys - memInfo.ullAvailPhys;

	return physMemUsed;
}

SIZE_T PerformanceStats::GetPhysicalMemoryUsedByEngine()
{
    PROCESS_MEMORY_COUNTERS_EX pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
    SIZE_T physMemUsedByMe = pmc.WorkingSetSize;

    return physMemUsedByMe;
}

SIZE_T PerformanceStats::GetPhysicalMemoryAvailable()
{
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	if (!GlobalMemoryStatusEx(&memInfo))
	{
		LogFatalError("Failed to get memory status.");
		return 0;
	}
	SIZE_T physMemAvailable = memInfo.ullAvailPhys;

	return physMemAvailable;
}