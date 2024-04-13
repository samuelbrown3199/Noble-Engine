#include "EditorUI.h"

#include <Engine\Useful.h>
#include <Engine\Core\Application.h>
#include <Engine\Core\Graphics\Renderer.h>
#include <Engine\Core\SceneManager.h>
#include <Engine\Core\EngineBehaviours\DebugCam.h>
#include <Engine\Core\InputManager.h>

#include <Engine\Core\ECS\Entity.hpp>

#include <Engine\Core\EngineResources\AudioClip.h>
#include <Engine\Core\EngineResources\Model.h>
#include <Engine\Core\EngineResources\Texture.h>
#include <Engine\Core\EngineResources\Script.h>

int EditorUI::m_iSelEntity = -1;
int EditorUI::m_iSelSystem = -1;

Entity* EditorUI::m_DebugCam = nullptr;

void EditorUI::ChangeEditorMode()
{
	Application::GetApplication()->SetPlayMode(!Application::GetApplication()->GetPlayMode());
}

void EditorUI::CreateEditorCam()
{
	if (!m_DebugCam)
	{
		m_DebugCam = Application::GetApplication()->CreateEntity();
		m_DebugCam->m_sEntityName = "Editor Cam";
		m_DebugCam->AddBehaviour<DebugCam>();
	}
	else
	{
		Application::GetApplication()->DeleteEntity(Application::GetApplication()->GetEntityIndex(m_DebugCam->m_sEntityID));
		m_DebugCam = nullptr;
	}
}

void EditorUI::InitializeInterface(ImGuiWindowFlags defaultFlags)
{
	EditorToolUI::InitializeInterface(defaultFlags);
}

void EditorUI::DoInterface()
{
	HandleShortcutInputs();

	ImGui::SetNextWindowSize(ImVec2(450, Renderer::GetScreenSize().y));

	NobleRegistry* registry = Application::GetApplication()->GetRegistry();

	ImGui::Begin("Editor", &m_uiOpen, m_windowFlags);

	std::string playModeButton = "Enter Play Mode";
	if(Application::GetApplication()->GetPlayMode())
		playModeButton = "Enter Edit Mode";
	if (ImGui::Button(playModeButton.c_str()))
	{
		ChangeEditorMode();
	}

	std::vector<Entity>& entities = Application::GetApplication()->GetEntityList();
	if(ImGui::Button("Create Entity"))
	{
		Application::GetApplication()->CreateEntity();
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete Entity"))
	{
		if(m_iSelEntity != -1)
			Application::GetApplication()->DeleteEntity(Application::GetApplication()->GetEntityIndex(entities.at(m_iSelEntity).m_sEntityID));
	}

	ImGui::SameLine();
	if (ImGui::Button("Add Child Entity"))
	{
		if (m_iSelEntity != -1)
			entities.at(m_iSelEntity).CreateChildObject();
	}

	if (ImGui::TreeNode("Entities"))
	{
		std::vector<std::pair<std::string, ComponentRegistry>>* compRegistry = registry->GetComponentRegistry();
		std::vector<std::pair<std::string, Behaviour*>>* behaviourRegistry = registry->GetBehaviourRegistry();

		static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
		static int selection_mask = (1 << 2);
		for (int i = 0; i < entities.size(); i++)
			entities.at(i).DoEntityInterface(compRegistry, behaviourRegistry, i, m_iSelEntity);

		ImGui::TreePop();
	}

	Renderer* renderer = Application::GetApplication()->GetRenderer();

	ImGui::Dummy(ImVec2(0.0f, 20.0f));

	clearColour = ImVec4(Renderer::GetClearColour().x, Renderer::GetClearColour().y, Renderer::GetClearColour().z, 200.0f / 255.0f);
	ImGui::ColorEdit3("Clear Colour", (float*)&clearColour);
	Renderer::SetClearColour(glm::vec3(clearColour.x, clearColour.y, clearColour.z));

	ambientColour = ImVec4(renderer->m_sceneData.ambientColour.x, renderer->m_sceneData.ambientColour.y, renderer->m_sceneData.ambientColour.z, renderer->m_sceneData.ambientColour.w);
	ImGui::ColorEdit4("Ambient Colour", (float*)&ambientColour);
	renderer->m_sceneData.ambientColour = glm::vec4(ambientColour.x, ambientColour.y, ambientColour.z, ambientColour.w);

	sunlightColour = ImVec4(renderer->m_sceneData.sunlightColour.x, renderer->m_sceneData.sunlightColour.y, renderer->m_sceneData.sunlightColour.z, renderer->m_sceneData.sunlightColour.w);
	ImGui::ColorEdit3("Sunlight Colour", (float*)&sunlightColour);
	renderer->m_sceneData.sunlightColour = glm::vec4(sunlightColour.x, sunlightColour.y, sunlightColour.z, 1.0f);

	float sunStrength = renderer->m_sceneData.sunlightDirection.w;
	ImGui::DragFloat("Sun Strength", &sunStrength, .1f, 0.0f, 1.0f);

	float sunDir[3] = { renderer->m_sceneData.sunlightDirection.x, renderer->m_sceneData.sunlightDirection.y, renderer->m_sceneData.sunlightDirection.z };
	ImGui::DragFloat3("Sunlight Direction", sunDir, 0.01f);
	renderer->m_sceneData.sunlightDirection = glm::vec4(sunDir[0], sunDir[1], sunDir[2], sunStrength);

	static float renderScale = renderer->GetRenderScale();
	ImGui::DragFloat("Render Scale", &renderScale, 0.1f, 0.1f, 1.0f, "%.2f");
	renderer->SetRenderScale(renderScale);

	ImGui::End();
}


void EditorUI::HandleShortcutInputs()
{
	if (InputManager::GetKey(SDLK_LCTRL) && InputManager::GetKeyDown(SDLK_q))
		CreateEditorCam();

	/*if (InputManager::GetKey(SDLK_LCTRL) && InputManager::GetKeyDown(SDLK_r))
		OpenResourceManager();

	if (InputManager::GetKey(SDLK_LCTRL) && InputManager::GetKeyDown(SDLK_p))
		OpenProfiler();*/

	if (InputManager::GetKey(SDLK_LCTRL) && InputManager::GetKeyDown(SDLK_RETURN))
		ChangeEditorMode();

	if (InputManager::GetKey(SDLK_LCTRL) && InputManager::GetKeyDown(SDLK_s))
		Application::GetApplication()->GetSceneManager()->SaveLoadedScene();
}