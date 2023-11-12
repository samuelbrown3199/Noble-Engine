#include "Profiler.h"


#include <vector>

#include <Engine\Core\Application.h>

void Profiler::InitializeInterface()
{
    m_pStats = Application::GetPerformanceStats();

    m_vFrameTimeStats.push_back(FrameTimeStat("Frame", &m_pStats->m_dPreviousFrameTime));
    m_vFrameTimeStats.push_back(FrameTimeStat("Pre-Update", &m_pStats->m_frameTimes[1]));
    m_vFrameTimeStats.push_back(FrameTimeStat("Update", &m_pStats->m_frameTimes[2]));
    m_vFrameTimeStats.push_back(FrameTimeStat("Render", &m_pStats->m_dPreviousRenderTime));
    m_vFrameTimeStats.push_back(FrameTimeStat("Cleanup", &m_pStats->m_frameTimes[4]));
}

void Profiler::DoInterface()
{
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(500.0f, 500.0f), ImGuiCond_Appearing);
    if (!ImGui::Begin("Profiler", &m_uiOpen))
    {
        ImGui::End();
        return;
    }

    float maxFrameTime = 0;

    for (int i = 0; i < m_vFrameTimeStats.size(); i++)
    {
        m_vFrameTimeStats[i].m_frameTimeArray[m_iMaxSamples] = {0};
        m_vFrameTimeStats[i].m_frameTimeArrayX[m_iMaxSamples] = { 0 };

        m_vFrameTimeStats[i].m_qTimes.push_back(*m_vFrameTimeStats[i].m_targetValue);
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

        if (m_vFrameTimeStats[i].m_fMaxFrameTime > maxFrameTime)
            maxFrameTime = m_vFrameTimeStats[i].m_fMaxFrameTime;
    }

    maxFrameTime = maxFrameTime > 20.0f ? maxFrameTime : 20.0f;

    m_qFrameRates.push_back(m_pStats->m_dFPS);
    if (m_qFrameRates.size() > m_iMaxSamples)
        m_qFrameRates.pop_front();

    static float frameRateArray[100] = { 0 };
    float maxFrameRate = 0;
    float avgFrameRate = 0;
    for (int i = 0; i < m_qFrameRates.size(); i++)
    {
        avgFrameRate += m_qFrameRates[i];
        frameRateArray[i] = m_qFrameRates[i];

        if (m_qFrameRates[i] > maxFrameRate)
            maxFrameRate = m_qFrameRates[i];
    }
    avgFrameRate /= m_qFrameRates.size();
    maxFrameRate = maxFrameRate > m_dMaxFPS ? maxFrameRate : m_dMaxFPS;

    std::string overlay = FormatString("Avg Frame Rate: %.2f", avgFrameRate);
    ImGui::PlotLines("", frameRateArray, IM_ARRAYSIZE(frameRateArray), 0, overlay.c_str(), 0, maxFrameRate, ImVec2(495.0f, 80.0f));

    ImPlotFlags plotFlags = ImPlotFlags_NoInputs;
    if (ImPlot::BeginPlot("Frame Times", ImVec2(-1,0), plotFlags))
    {
        ImPlotShadedFlags flags = 0;

        ImPlot::SetupAxes(nullptr, nullptr, flags, flags);
        ImPlot::SetupAxisLimits(ImAxis_X1, 0, m_iMaxSamples, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0, maxFrameTime);
        ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5f);

        float fill_ref = 0;
        int shade_mode = 1;

        ImPlot::PlotShaded(m_vFrameTimeStats[0].m_sName.c_str(), m_vFrameTimeStats[0].m_frameTimeArrayX, m_vFrameTimeStats[0].m_frameTimeArray, 100);

        for (int i = 1; i < m_vFrameTimeStats.size(); i++)
        {
            ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
            ImPlot::PlotLine(m_vFrameTimeStats[i].m_sName.c_str(), m_vFrameTimeStats[i].m_frameTimeArrayX, m_vFrameTimeStats[i].m_frameTimeArray, 100);
        }
        ImPlot::EndPlot();
    }

    ImGui::End();
}