#include "Profiler.h"


#include <vector>

#include <Engine\Core\Application.h>

void Profiler::ProcessData()
{
    for (int i = 0; i < m_vFrameTimeStats.size(); i++)
    {
        m_vFrameTimeStats[i].m_frameTimeArray[m_iMaxSamples] = { 0 };
        m_vFrameTimeStats[i].m_frameTimeArrayX[m_iMaxSamples] = { 0 };

        m_vFrameTimeStats[i].m_qTimes.push_back(m_pStats->GetPerformanceMeasurementInMicroSeconds(m_vFrameTimeStats[i].m_sName) / 1000);
        if (m_vFrameTimeStats[i].m_qTimes.size() > m_iMaxSamples)
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

    m_fMaxFrameTime = m_fMaxFrameTime > 20.0f ? m_fMaxFrameTime : 20.0f;

    m_qFrameRates.push_back(m_pStats->m_dFPS);
    if (m_qFrameRates.size() > m_iMaxSamples)
        m_qFrameRates.pop_front();

    for (int i = 0; i < m_qFrameRates.size(); i++)
    {
        m_fAvgFrameRate += m_qFrameRates[i];
        m_fFrameRateArray[i] = m_qFrameRates[i];

        if (m_qFrameRates[i] > m_fMaxFrameRate)
            m_fMaxFrameRate = m_qFrameRates[i];
    }
    m_fAvgFrameRate /= m_qFrameRates.size();
    m_fMaxFrameRate = m_fMaxFrameRate > m_dMaxFPS ? m_fMaxFrameRate : m_dMaxFPS;

    for (int i = 0; i < m_vMemoryStats.size(); i++)
    {
        m_vMemoryStats[i].m_dMemoryValuesX[m_iMaxSamples] = { 0 };

        m_vMemoryStats[i].m_qMemoryValues.push_back(*m_vMemoryStats[i].m_fTargetValue);
        if (m_vMemoryStats[i].m_qMemoryValues.size() > m_iMaxSamples)
            m_vMemoryStats[i].m_qMemoryValues.pop_front();

        for (int o = 0; o < m_vMemoryStats[i].m_qMemoryValues.size(); o++)
        {
            m_vMemoryStats[i].m_dMemoryValues[o] = m_vMemoryStats[i].m_qMemoryValues[o];
            m_vMemoryStats[i].m_dMemoryValuesX[o] = o;

            if (m_vMemoryStats[i].m_qMemoryValues[o] > m_fMaxMemory)
				m_fMaxMemory = m_vMemoryStats[i].m_qMemoryValues[o];
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

    m_vMemoryStats.push_back(MemoryStat("Physical Memory Used", &m_pStats->m_fPhysicalMemoryUsageByEngine));
    m_vMemoryStats.push_back(MemoryStat("Virtual Memory Used", &m_pStats->m_fVirtualMemoryUsageByEngine));
}

void Profiler::DoInterface()
{
    Renderer* renderer = Application::GetApplication()->GetRenderer();
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
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
        ImPlot::SetupAxisLimits(ImAxis_X1, 0, m_iMaxSamples, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0, m_fMaxFrameTime);
        ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5f);

        ImPlot::PlotShaded(m_vFrameTimeStats[0].m_sName.c_str(), m_vFrameTimeStats[0].m_frameTimeArrayX, m_vFrameTimeStats[0].m_frameTimeArray, 100);

        for (int i = 1; i < m_vFrameTimeStats.size(); i++)
        {
            ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
            ImPlot::PlotLine(m_vFrameTimeStats[i].m_sName.c_str(), m_vFrameTimeStats[i].m_frameTimeArrayX, m_vFrameTimeStats[i].m_frameTimeArray, 100);
        }
        ImPlot::EndPlot();
    }

    ImGui::SeparatorText("Memory Usage");
    if (ImPlot::BeginPlot("Memory Usage", ImVec2(-1, 0), plotFlags))
    {
        ImPlotShadedFlags flags = 0;
        ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 1.0f);

        ImPlot::SetupAxes(nullptr, nullptr, flags, flags);
        ImPlot::SetupAxisLimits(ImAxis_X1, 0, m_iMaxSamples, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0, m_fMaxMemory);
        ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5f);

        for (int i = 0; i < m_vMemoryStats.size(); i++)
        {
            ImPlot::PlotShaded(m_vMemoryStats[i].m_sName.c_str(), m_vMemoryStats[i].m_dMemoryValuesX, m_vMemoryStats[i].m_dMemoryValues, 100);
        }

        ImPlot::EndPlot();
    }

    std::string memoryAvailableString = FormatString("Total Physical Memory: %.2fMB | Physical Memory Available: %.2fMB | Total Virtual Memory: %.2fMB | Virtual Memory Available: %.2fMB", m_pStats->m_fTotalPhysicalMemory, m_pStats->m_fPhysicalMemoryAvailable, m_pStats->m_fTotalVirtualMemory, m_pStats->m_fVirtualMemoryAvailable);
    ImGui::Text(memoryAvailableString.c_str());

    std::string memoryUsageString = FormatString("Physical Memory Used By Engine : % .2fMB | Virtual Memory Used By Engine : % .2fMB", m_pStats->m_fPhysicalMemoryUsageByEngine, m_pStats->m_fVirtualMemoryUsageByEngine);
    ImGui::Text(memoryUsageString.c_str());

    ImGui::SeparatorText("Renderer Stats");
    ImGui::Text(FormatString("Scene Render Objects: %d. On Screen Objects: %d.", renderer->GetRenderableCount(), renderer->GetOnScreenRenderableCount()).c_str());

    int vertCount = 0;
    int triCount = 0;
    renderer->GetOnScreenVerticesAndTriangles(vertCount, triCount);
    ImGui::Text(FormatString("Rendered Vertices: %d. Rendered Triangles: %d.", vertCount, triCount).c_str());

    UpdateWindowState();

    ImGui::End();
}