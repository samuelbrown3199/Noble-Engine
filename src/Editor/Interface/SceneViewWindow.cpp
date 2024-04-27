#include "SceneViewWindow.h"

#include <Engine/Core/Application.h>
#include <Engine/Maths/Raycaster.h>	
#include <Engine/Core/EngineComponents/MeshRenderer.h>
#include <Engine/Core/InputManager.h>

void SceneViewWindow::InitializeInterface(ImGuiWindowFlags defaultFlags)
{
	EditorToolUI::InitializeInterface(defaultFlags);

	Renderer* renderer = Application::GetApplication()->GetRenderer();
	renderer->SetDrawWindowSet(&m_drawWindowSet);
	renderer->SetDrawToWindow(true);
}

void SceneViewWindow::DoInterface()
{
	ImGui::Begin("Scene View", &m_uiOpen, m_windowFlags);
	UpdateWindowState();

	if (m_bWindowFocused && m_bWindowHovered && ImGui::IsMouseClicked(0))
	{
		Ray ray = Raycaster::GetRayToMousePosition(glm::vec2(m_windowSize.x, m_windowSize.y), glm::vec2(m_relativeMousePos.x, m_relativeMousePos.y));
		InputManager* inputManager = Application::GetApplication()->GetInputManager();

		if (ray.m_hitObject && inputManager->GetKey(SDLK_r))
		{
			ResourceManager* rManager = Application::GetApplication()->GetResourceManager();

			LogInfo(FormatString("Hit entity %s! Position %.2f %.2f %.2f", ray.m_hitEntity->m_sEntityID.c_str(), ray.m_hitPosition.x, ray.m_hitPosition.y, ray.m_hitPosition.z));

			Entity* ent = Application::GetApplication()->CreateEntity();
			ent->AddComponent<Transform>(ray.m_hitPosition);
			MeshRenderer* mr = ent->AddComponent<MeshRenderer>();
			mr->m_model = rManager->LoadResource<Model>("\\GameData\\Models\\cube.obj");
			mr->m_texture = rManager->LoadResource<Texture>("\\GameData\\Textures\\cottage_diffuse.png");
			mr->m_pipeline = rManager->LoadResource<Pipeline>("Standard");
		}
	}
	ImGui::Image(m_drawWindowSet, ImVec2(m_windowSize.x-15, m_windowSize.y - 35));

	ImGui::End();
}