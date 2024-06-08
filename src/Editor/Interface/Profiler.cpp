#include "Profiler.h"


#include <vector>

#include <Engine\Core\Application.h>
#include <Engine\Core\Registry.h>

void Profiler::ProcessData()
{
    m_fMaxMemory = 0;
    m_fMaxFrameRate = 0;
    m_fMaxFrameTime = 0;

    for (int i = 0; i < m_vFrameTimeStats.size(); i++)
    {
        m_vFrameTimeStats[i].m_frameTimeArray[NobleProfiling::m_iMaxSamples] = { 0 };
        m_vFrameTimeStats[i].m_frameTimeArrayX[NobleProfiling::m_iMaxSamples] = { 0 };

        m_vFrameTimeStats[i].m_qTimes.push_back(m_pStats->GetPerformanceMeasurement(m_vFrameTimeStats[i].m_sName)->GetPerformanceMeasurementInMicroSeconds() / 1000);
        if (m_vFrameTimeStats[i].m_qTimes.size() > NobleProfiling::m_iMaxSamples)
            m_vFrameTimeStats[i].m_qTimes.pop_front();

        for (int o = 0; o < m_vFrameTimeStats[i].m_qTimes.size(); o++)
        {
            m_vFrameTimeStats[i].m_fAvgFrameTime += m_vFrameTimeStats[i].m_qTimes[o];
            m_vFrameTimeStats[i].m_frameTimeArray[o] = m_vFrameTimeStats[i].m_qTimes[o];
            m_vFrameTimeStats[i].m_frameTimeArrayX[o] = o;

            if (m_vFrameTimeStats[i].m_qTimes[o] > m_vFrameTimeStats[i].m_fMaxFrameTime)
                m_vFrameTimeStats[i].m_fMaxFrameTime = m_vFrameTimeStats[i].m_qTimes[o];
        }
        m_vFrameTimeStats[i].m_fAvgFrameTime /= m_vFrameTimeStats[i].m_qTimes.size();

        if (m_vFrameTimeStats[i].m_fMaxFrameTime > m_fMaxFrameTime)
            m_fMaxFrameTime = m_vFrameTimeStats[i].m_fMaxFrameTime;
    }

    std::map<std::string, PerformanceMeasurement>* updateTimes = m_pStats->GetSystemUpdateTimes();
    std::map<std::string, PerformanceMeasurement>* renderTimes = m_pStats->GetSystemRenderTimes();
    for (int i = 0; i < m_vComponentUpdateTimes.size(); i++)
    {
        m_vComponentUpdateTimes[i].m_frameTimeArray[NobleProfiling::m_iMaxSamples] = { 0 };
        m_vComponentUpdateTimes[i].m_frameTimeArrayX[NobleProfiling::m_iMaxSamples] = { 0 };

        m_vComponentRenderTimes[i].m_frameTimeArray[NobleProfiling::m_iMaxSamples] = { 0 };
        m_vComponentRenderTimes[i].m_frameTimeArrayX[NobleProfiling::m_iMaxSamples] = { 0 };

        m_vComponentUpdateTimes[i].m_qTimes.push_back(updateTimes->at(m_vComponentUpdateTimes[i].m_sName).m_measurementTime.count() / 1000);
        if (m_vComponentUpdateTimes[i].m_qTimes.size() > NobleProfiling::m_iMaxSamples)
			m_vComponentUpdateTimes[i].m_qTimes.pop_front();

        m_vComponentRenderTimes[i].m_qTimes.push_back(renderTimes->at(m_vComponentRenderTimes[i].m_sName).m_measurementTime.count() / 1000);
        if (m_vComponentRenderTimes[i].m_qTimes.size() > NobleProfiling::m_iMaxSamples) 
            m_vComponentRenderTimes[i].m_qTimes.pop_front();

        for (int o = 0; o < m_vComponentUpdateTimes[i].m_qTimes.size(); o++)
		{
			m_vComponentUpdateTimes[i].m_fAvgFrameTime += m_vComponentUpdateTimes[i].m_qTimes[o];
			m_vComponentUpdateTimes[i].m_frameTimeArray[o] = m_vComponentUpdateTimes[i].m_qTimes[o];
			m_vComponentUpdateTimes[i].m_frameTimeArrayX[o] = o;

			if (m_vComponentUpdateTimes[i].m_qTimes[o] > m_vComponentUpdateTimes[i].m_fMaxFrameTime)
				m_vComponentUpdateTimes[i].m_fMaxFrameTime = m_vComponentUpdateTimes[i].m_qTimes[o];
		}
		m_vComponentUpdateTimes[i].m_fAvgFrameTime /= m_vComponentUpdateTimes[i].m_qTimes.size();

		if (m_vComponentUpdateTimes[i].m_fMaxFrameTime > m_fMaxFrameTime)
			m_fMaxFrameTime = m_vComponentUpdateTimes[i].m_fMaxFrameTime;

        for (int o = 0; o < m_vComponentRenderTimes[i].m_qTimes.size(); o++)
        {
			m_vComponentRenderTimes[i].m_fAvgFrameTime += m_vComponentRenderTimes[i].m_qTimes[o];
			m_vComponentRenderTimes[i].m_frameTimeArray[o] = m_vComponentRenderTimes[i].m_qTimes[o];
			m_vComponentRenderTimes[i].m_frameTimeArrayX[o] = o;

			if (m_vComponentRenderTimes[i].m_qTimes[o] > m_vComponentRenderTimes[i].m_fMaxFrameTime)
				m_vComponentRenderTimes[i].m_fMaxFrameTime = m_vComponentRenderTimes[i].m_qTimes[o];
		}
        m_vComponentRenderTimes[i].m_fAvgFrameTime /= m_vComponentRenderTimes[i].m_qTimes.size();

        if (m_vComponentRenderTimes[i].m_fMaxFrameTime > m_fMaxFrameTime)
            m_fMaxFrameTime = m_vComponentRenderTimes[i].m_fMaxFrameTime;
	}

    m_fMaxFrameTime = m_fMaxFrameTime > 20.0f ? m_fMaxFrameTime+(m_fMaxFrameTime/10) : 20.0f;

    m_qFrameRates.push_back(m_pStats->m_dFPS);
    if (m_qFrameRates.size() > NobleProfiling::m_iMaxSamples)
        m_qFrameRates.pop_front();

    for (int i = 0; i < m_qFrameRates.size(); i++)
    {
        m_fAvgFrameRate += m_qFrameRates[i];
        m_fFrameRateArray[i] = m_qFrameRates[i];

        if (m_qFrameRates[i] > m_fMaxFrameRate)
            m_fMaxFrameRate = m_qFrameRates[i];
    }
    m_fAvgFrameRate /= m_qFrameRates.size();
    m_fMaxFrameRate = m_fMaxFrameRate > m_dMaxFPS ? m_fMaxFrameRate+(m_fMaxFrameRate/10) : m_dMaxFPS;

    for (int i = 0; i < m_vMainMemoryStats.size(); i++)
    {
        m_vMainMemoryStats[i].m_dMemoryValuesX[NobleProfiling::m_iMaxSamples] = { 0 };

        m_vMainMemoryStats[i].m_qMemoryValues.push_back(*m_vMainMemoryStats[i].m_fTargetValue);
        if (m_vMainMemoryStats[i].m_qMemoryValues.size() > NobleProfiling::m_iMaxSamples)
            m_vMainMemoryStats[i].m_qMemoryValues.pop_front();

        for (int o = 0; o < m_vMainMemoryStats[i].m_qMemoryValues.size(); o++)
        {
            m_vMainMemoryStats[i].m_dMemoryValues[o] = m_vMainMemoryStats[i].m_qMemoryValues[o];
            m_vMainMemoryStats[i].m_dMemoryValuesX[o] = o;

            if (m_vMainMemoryStats[i].m_qMemoryValues[o] > m_fMaxMemory)
                m_fMaxMemory = m_vMainMemoryStats[i].m_qMemoryValues[o];
        }
    }
    m_fMaxMemory = m_fMaxMemory + (m_fMaxMemory / 10);
    std::sort(m_vMainMemoryStats.begin(), m_vMainMemoryStats.end(), [](MemoryStat a, MemoryStat b) { return *a.m_fTargetValue > *b.m_fTargetValue; });

    for (int i = 0; i < m_vMemoryStats.size(); i++)
    {
        m_vMemoryStats[i].m_dMemoryValuesX[NobleProfiling::m_iMaxSamples] = { 0 };

        m_vMemoryStats[i].m_qMemoryValues.push_back(*m_vMemoryStats[i].m_fTargetValue);
        if (m_vMemoryStats[i].m_qMemoryValues.size() > NobleProfiling::m_iMaxSamples)
            m_vMemoryStats[i].m_qMemoryValues.pop_front();

        for (int o = 0; o < m_vMemoryStats[i].m_qMemoryValues.size(); o++)
        {
            m_vMemoryStats[i].m_dMemoryValues[o] = m_vMemoryStats[i].m_qMemoryValues[o];
            m_vMemoryStats[i].m_dMemoryValuesX[o] = o;

            //if I add a max memory value for this do it here.
        }
    }

    std::sort(m_vMemoryStats.begin(), m_vMemoryStats.end(), [](MemoryStat a, MemoryStat b) { return *a.m_fTargetValue > *b.m_fTargetValue; });
}

void Profiler::InitializeInterface(ImGuiWindowFlags defaultFlags)
{
    EditorToolUI::InitializeInterface(defaultFlags);
    m_pStats = Application::GetApplication()->GetPerformanceStats();

    m_vFrameTimeStats.push_back(FrameTimeStat("Frame"));
    m_vFrameTimeStats.push_back(FrameTimeStat("Pre-Update"));
    m_vFrameTimeStats.push_back(FrameTimeStat("Update"));
    m_vFrameTimeStats.push_back(FrameTimeStat("Render"));
    m_vFrameTimeStats.push_back(FrameTimeStat("EditorUpdate"));
    m_vFrameTimeStats.push_back(FrameTimeStat("EditorRender"));
    m_vFrameTimeStats.push_back(FrameTimeStat("Cleanup"));

    m_vMainMemoryStats.push_back(MemoryStat("Physical Memory Used", &m_pStats->m_fPhysicalMemoryUsageByEngine));
    m_vMainMemoryStats.push_back(MemoryStat("Virtual Memory Used", &m_pStats->m_fVirtualMemoryUsageByEngine));

    std::vector<std::pair<std::string, ComponentRegistry>>* compRegistry = Application::GetApplication()->GetRegistry()->GetComponentRegistry();
    for (int i = 0; i < compRegistry->size(); i++)
    {
        m_vMemoryStats.push_back(MemoryStat(compRegistry->at(i).first, &compRegistry->at(i).second.m_fDataListMemoryUsage));
    }
}

void Profiler::DoInterface()
{
    Renderer* renderer = Application::GetApplication()->GetRenderer();
    std::vector<std::pair<std::string, ComponentRegistry>>* compRegistry = Application::GetApplication()->GetRegistry()->GetComponentRegistry();

    if (!ImGui::Begin("Profiler", &m_uiOpen, m_windowFlags))
    {
        ImGui::End();
        return;
    }

    ProcessData();

    std::string overlay = FormatString("Avg Frame Rate: %.2f", m_fAvgFrameRate);
    ImGui::PlotLines("", m_fFrameRateArray, IM_ARRAYSIZE(m_fFrameRateArray), 0, overlay.c_str(), 0, m_fMaxFrameRate, ImVec2(-1, 80.0f));

    ImPlotFlags plotFlags = ImPlotFlags_NoInputs;
    if (ImPlot::BeginPlot("Frame Times", ImVec2(-1,0), plotFlags))
    {
        ImPlotShadedFlags flags = 0;

        ImPlot::SetupAxes(nullptr, nullptr, flags, flags);
        ImPlot::SetupAxisLimits(ImAxis_X1, 0, NobleProfiling::m_iMaxSamples, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0, m_fMaxFrameTime);
        ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5f);

        ImPlot::PlotShaded(m_vFrameTimeStats[0].m_sName.c_str(), m_vFrameTimeStats[0].m_frameTimeArrayX, m_vFrameTimeStats[0].m_frameTimeArray, NobleProfiling::m_iMaxSamples);

        for (int i = 1; i < m_vFrameTimeStats.size(); i++)
        {
            ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
            ImPlot::PlotLine(m_vFrameTimeStats[i].m_sName.c_str(), m_vFrameTimeStats[i].m_frameTimeArrayX, m_vFrameTimeStats[i].m_frameTimeArray, NobleProfiling::m_iMaxSamples);
        }
        ImPlot::EndPlot();
    }
    ImGui::Dummy(ImVec2(0.0f, 5.0f));
    for (int i = 0; i < m_vFrameTimeStats.size(); i++)
    {
        ImGui::Text(FormatString("%s Avg Frame Time: %.2fms", m_vFrameTimeStats[i].m_sName.c_str(), m_vFrameTimeStats[i].m_fAvgFrameTime).c_str());
    }

    ImGui::SeparatorText("Component Frame Times");
    std::map<std::string, PerformanceMeasurement>* updateTimes = m_pStats->GetSystemUpdateTimes();
    std::map<std::string, PerformanceMeasurement>* renderTimes = m_pStats->GetSystemRenderTimes();
    for (int i = 0; i < compRegistry->size(); i++)
    {
        double updateTime = 0;
        for (int o = 0; o < m_vComponentUpdateTimes.size(); o++)
        {
            if (m_vComponentUpdateTimes[o].m_sName == compRegistry->at(i).first)
            {
                updateTime = m_vComponentUpdateTimes[o].m_fAvgFrameTime;
                break;
            }
        }
        double renderTime = 0;
        for (int o = 0; o < m_vComponentRenderTimes.size(); o++)
        {
            if (m_vComponentRenderTimes[o].m_sName == compRegistry->at(i).first)
            {
                renderTime = m_vComponentRenderTimes[o].m_fAvgFrameTime;
                break;
            }
        }

        ImGui::Text(FormatString("%s Avg Update Time: %.2fms | Avg Render Time: %.2fms", compRegistry->at(i).first.c_str(), updateTime, renderTime).c_str());
    }

    ImGui::SeparatorText("Memory Usage");
    if (ImPlot::BeginPlot("Memory Usage", ImVec2(-1, 0), plotFlags))
    {
        ImPlotShadedFlags flags = 0;
        ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 1.0f);

        ImPlot::SetupAxes(nullptr, nullptr, flags, flags);
        ImPlot::SetupAxisLimits(ImAxis_X1, 0, NobleProfiling::m_iMaxSamples, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0, m_fMaxMemory);
        ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5f);

        for (int i = 0; i < m_vMainMemoryStats.size(); i++)
        {
            ImPlot::PlotShaded(m_vMainMemoryStats[i].m_sName.c_str(), m_vMainMemoryStats[i].m_dMemoryValuesX, m_vMainMemoryStats[i].m_dMemoryValues, NobleProfiling::m_iMaxSamples);
        }

        ImPlot::EndPlot();
    }

    ImGui::Dummy(ImVec2(0.0f, 5.0f));
    std::string memoryAvailableString = FormatString("Total Physical Memory: %.2fMB | Physical Memory Available: %.2fMB | Total Virtual Memory: %.2fMB | Virtual Memory Available: %.2fMB", m_pStats->m_fTotalPhysicalMemory, m_pStats->m_fPhysicalMemoryAvailable, m_pStats->m_fTotalVirtualMemory, m_pStats->m_fVirtualMemoryAvailable);
    ImGui::Text(memoryAvailableString.c_str());

    std::string memoryUsageString = FormatString("Physical Memory Used By Engine: %.2fMB | Virtual Memory Used By Engine: %.2fMB", m_pStats->m_fPhysicalMemoryUsageByEngine, m_pStats->m_fVirtualMemoryUsageByEngine);
    ImGui::Text(memoryUsageString.c_str());

    ImGui::SeparatorText("Detailed Memory Usage");
    std::vector<std::string> memoryUsageNames;
    std::vector<float> memoryUsageValues;

    memoryUsageNames.clear();
    memoryUsageValues.clear();

    for (int i = 0; i < m_vMemoryStats.size(); i++)
    {
        std::string componentUsageString = FormatString("%s Memory Used: %.2fMB", m_vMemoryStats.at(i).m_sName.c_str(), *m_vMemoryStats.at(i).m_fTargetValue);
        ImGui::Text(componentUsageString.c_str());

        memoryUsageNames.push_back(m_vMemoryStats.at(i).m_sName);
        memoryUsageValues.push_back(*m_vMemoryStats.at(i).m_fTargetValue);
    }
    //TODO: Add Pie chart here for visual representation of memory usage.

    ImGui::SeparatorText("Renderer Stats");
    ImGui::Text(FormatString("Scene Render Objects: %d. On Screen Objects: %d.", renderer->GetRenderableCount(), renderer->GetOnScreenRenderableCount()).c_str());

    int vertCount = 0;
    int triCount = 0;
    renderer->GetOnScreenVerticesAndTriangles(vertCount, triCount);
    ImGui::Text(FormatString("Rendered Vertices: %d. Rendered Triangles: %d.", vertCount, triCount).c_str());

    UpdateWindowState();

    ImGui::End();
}

void Profiler::AddComponentMeasurement(std::string name)
{
    m_vComponentUpdateTimes.push_back(FrameTimeStat(name));
	m_vComponentRenderTimes.push_back(FrameTimeStat(name));
}