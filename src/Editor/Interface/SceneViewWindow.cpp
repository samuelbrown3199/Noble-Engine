#include "SceneViewWindow.h"

#include <Engine/Core/Application.h>
#include <Engine/Maths/Raycaster.h>	


void SceneViewWindow::DoInterface()
{
	Renderer* renderer = Application::GetApplication()->GetRenderer();
	renderer->SetDrawWindowSet(&m_drawWindowSet);
	renderer->SetDrawToWindow(true);

	ImGui::Begin("Scene View", &m_uiOpen, m_windowFlags);

	UpdateWindowState();

	if (m_bWindowFocused && m_bWindowHovered && ImGui::IsMouseClicked(0))
	{
		Ray ray = Raycaster::GetRayToMousePosition(glm::vec2(m_windowSize.x, m_windowSize.y), glm::vec2(m_relativeMousePos.x, m_relativeMousePos.y));
		if (ray.m_hitObject)
		{
			LogInfo("Hit Object: " + ray.m_hitEntity->m_sEntityID);
		}
	}

	ImGui::Image(m_drawWindowSet, ImVec2(m_windowSize.x-15, m_windowSize.y - 35));

	ImGui::End();
}