#include "PerformanceStats.h"
#include "Application.h"
#include "../Useful.h"
#include "Registry.h"

#include "psapi.h"

double PerformanceStats::m_dDeltaT;

void PerformanceStats::UpdatePerformanceStats()
{
	double frameMilliseconds = m_mPerformanceMeasurements["Frame"].GetPerformanceMeasurementInMicroSeconds() / 1000;
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

	m_mSystemUpdateTimes = std::map<std::string, PerformanceMeasurement>();
	m_mSystemRenderTimes = std::map<std::string, PerformanceMeasurement>();

	m_iCurrentFrameCount = 0;
}

void PerformanceStats::LogPerformanceStats()
{
	if (!m_bLogPerformance)
		return;

	if (m_iCurrentFrameCount != 0)
		return;

	std::string performanceStatsString = FormatString("FPS: %.2f | Delta Time: %.5f", m_dFPS, m_dDeltaT);

	std::map<std::string, PerformanceMeasurement>::iterator it = m_mPerformanceMeasurements.begin();
	for(; it != m_mPerformanceMeasurements.end(); it++)
	{
		performanceStatsString += FormatString(" | %s: %.2f", it->first, it->second.GetPerformanceMeasurementInMicroSeconds() / 1000);
	}
	performanceStatsString += "\n";
	for (it = m_mSystemUpdateTimes.begin(); it != m_mSystemUpdateTimes.end(); it++)
	{
		performanceStatsString += FormatString("%s Update Time: %.2f | Render Time: %.2f\n", it->first, it->second.m_measurementTime.count()/1000, m_mSystemRenderTimes.at(it->first).m_measurementTime.count() / 1000);
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
	if (m_mPerformanceMeasurements.count(name) != 0)
		LogFatalError("Tried to add duplicate performance measurement.");

	m_mPerformanceMeasurements[name] = PerformanceMeasurement();
}

void PerformanceStats::StartPerformanceMeasurement(std::string name)
{
	m_mPerformanceMeasurements[name].StartMeasurement();
}

void PerformanceStats::EndPerformanceMeasurement(std::string name)
{
	m_mPerformanceMeasurements[name].EndMeasurement();
}

void PerformanceStats::AddComponentMeasurement(std::string name)
{
	if (m_mSystemUpdateTimes.count(name) != 0)
		LogFatalError("Tried to add duplicate performance measurement.");

	m_mSystemUpdateTimes[name] = PerformanceMeasurement();
	m_mSystemRenderTimes[name] = PerformanceMeasurement();

	if(Application::GetApplication()->GetEditor() != nullptr)
		Application::GetApplication()->GetEditor()->AddComponentMeasurement(name);
}

void PerformanceStats::StartComponentMeasurement(std::string name, bool update)
{
	std::map<std::string, PerformanceMeasurement>* targetList = &m_mSystemUpdateTimes;
	if (!update)
		targetList = &m_mSystemRenderTimes;

	targetList->at(name).StartMeasurement();
}

void PerformanceStats::EndComponentMeasurement(std::string name, bool update)
{
	std::map<std::string, PerformanceMeasurement>* targetList = &m_mSystemUpdateTimes;
	if (!update)
		targetList = &m_mSystemRenderTimes;

	targetList->at(name).EndMeasurement();
}

PerformanceMeasurement* PerformanceStats::GetComponentUpdateMeasurement(std::string name)
{
	return &m_mSystemUpdateTimes[name];
}

PerformanceMeasurement* PerformanceStats::GetComponentRenderMeasurement(std::string name)
{
	return &m_mSystemRenderTimes[name];
}

PerformanceMeasurement* PerformanceStats::GetPerformanceMeasurement(std::string name)
{
	return &m_mPerformanceMeasurements[name];
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

	//--------------------Other Memory Usage Breakdown--------------------

	m_fResourceMemoryUsage = Application::GetApplication()->GetResourceManager()->GetResourceMemoryUsage();
	m_fEntityMemoryUsage = Application::GetApplication()->GetEntityMemoryUsage();

	m_fOtherMemoryUsage = m_fPhysicalMemoryUsageByEngine;
	m_fOtherMemoryUsage -= m_fResourceMemoryUsage;
	m_fOtherMemoryUsage -= m_fEntityMemoryUsage;

	std::vector<std::pair<std::string, ComponentRegistry>>* compRegistry = Application::GetApplication()->GetRegistry()->GetComponentRegistry();
	for (int i = 0; i < compRegistry->size(); i++)
	{
		m_fOtherMemoryUsage -= compRegistry->at(i).second.m_fDataListMemoryUsage;
	}
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