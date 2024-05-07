#include "SceneViewWindow.h"

#include <Engine/Core/Application.h>
#include <Engine/Maths/Raycaster.h>	
#include <Engine/Core/EngineComponents/MeshRenderer.h>
#include <Engine/Core/InputManager.h>

#include "../EditorManagement/EditorManager.h"
#include "SceneHierarchyWindow.h"

void SceneViewWindow::InitializeInterface(ImGuiWindowFlags defaultFlags)
{
	EditorToolUI::InitializeInterface(defaultFlags);

	Renderer* renderer = Application::GetApplication()->GetRenderer();
	renderer->SetDrawWindowSet(&m_drawWindowSet);
	renderer->SetDrawToWindow(true);

	m_drawWindowImage = renderer->GetDrawImage();
}

void SceneViewWindow::DoInterface()
{
	EditorManager* editorManager = dynamic_cast<EditorManager*>(m_pEditor);

	ImGui::Begin("Scene View", &m_uiOpen, m_windowFlags);
	UpdateWindowState();

	ImVec2 contentSize = ImGui::GetWindowContentRegionMax();
	contentSize.x -= 8;
	contentSize.y -= 19;

	Renderer* renderer = Application::GetApplication()->GetRenderer();
	renderer->SetDrawWindowSize(contentSize);

	float fRenderScale = renderer->GetRenderScale();

	if (m_bWindowFocused && m_bWindowHovered && ImGui::IsMouseClicked(0))
	{
		Ray ray = Raycaster::GetRayToMousePosition(glm::vec2(contentSize.x/ fRenderScale, contentSize.y / fRenderScale), glm::vec2(m_relativeMousePos.x - 8, m_relativeMousePos.y - 27));
		InputManager* inputManager = Application::GetApplication()->GetInputManager();

		if (ray.m_hitObject)
		{
			dynamic_cast<SceneHierarchyWindow*>(editorManager->GetEditorUI("SceneHierarchy"))->SetSelectedEntity(ray.m_hitEntity->m_sEntityID);
		}
	}
	ImGui::Image(m_drawWindowSet, ImVec2(contentSize.x / fRenderScale, contentSize.y / fRenderScale));

	ImGui::End();
}