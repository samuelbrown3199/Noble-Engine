#include "EditorUI.h"

#include <Engine\Useful.h>
#include <Engine\Core\Application.h>
#include <Engine\Core\Graphics\Renderer.h>
#include <Engine\Core\SceneManager.h>
#include <Engine\Core\InputManager.h>

#include <Engine\Core\ECS\Entity.h>

#include <Engine\Core\EngineResources\AudioClip.h>
#include <Engine\Core\EngineResources\Model.h>
#include <Engine\Core\EngineResources\Texture.h>
#include <Engine\Core\EngineResources\Script.h>

void EditorUI::ChangeEditorMode()
{
	Application::GetApplication()->SetPlayMode(!Application::GetApplication()->GetPlayMode());
}

void EditorUI::InitializeInterface(ImGuiWindowFlags defaultFlags)
{
	EditorToolUI::InitializeInterface(defaultFlags);
}

void EditorUI::DoInterface()
{
	Renderer* renderer = Application::GetApplication()->GetRenderer();

	HandleShortcutInputs();
	ImGui::SetNextWindowSize(ImVec2(450, renderer->GetScreenSize().y));

	NobleRegistry* registry = Application::GetApplication()->GetRegistry();

	ImGui::Begin("Editor", &m_uiOpen, m_windowFlags);

	UpdateWindowState();

	std::string playModeButton = "Enter Play Mode";
	if(Application::GetApplication()->GetPlayMode())
		playModeButton = "Enter Edit Mode";
	if (ImGui::Button(playModeButton.c_str()))
	{
		ChangeEditorMode();
	}

	ImGui::Dummy(ImVec2(0.0f, 20.0f));

	clearColour = ImVec4(renderer->GetClearColour().x, renderer->GetClearColour().y, renderer->GetClearColour().z, 200.0f / 255.0f);
	ImGui::ColorEdit3("Clear Colour", (float*)&clearColour);
	renderer->SetClearColour(glm::vec3(clearColour.x, clearColour.y, clearColour.z));

	ambientColour = ImVec4(renderer->m_sceneData.ambientColour.x, renderer->m_sceneData.ambientColour.y, renderer->m_sceneData.ambientColour.z, renderer->m_sceneData.ambientColour.w);
	ImGui::ColorEdit4("Ambient Colour", (float*)&ambientColour);
	renderer->m_sceneData.ambientColour = glm::vec4(ambientColour.x, ambientColour.y, ambientColour.z, ambientColour.w);

	static float renderScale = renderer->GetRenderScale();
	ImGui::DragFloat("Render Scale", &renderScale, 0.1f, 0.1f, 1.0f, "%.2f");
	renderer->SetRenderScale(renderScale);

	ImGui::End();
}


void EditorUI::HandleShortcutInputs()
{
	InputManager* inputManager = Application::GetApplication()->GetInputManager();

	/*if (InputManager::GetKey(SDLK_LCTRL) && InputManager::GetKeyDown(SDLK_r))
		OpenResourceManager();

	if (InputManager::GetKey(SDLK_LCTRL) && InputManager::GetKeyDown(SDLK_p))
		OpenProfiler();*/

	if (inputManager->GetKey(SDLK_LCTRL) && inputManager->GetKeyDown(SDLK_RETURN))
		ChangeEditorMode();

	if (inputManager->GetKey(SDLK_LCTRL) && inputManager->GetKeyDown(SDLK_s))
		Application::GetApplication()->GetSceneManager()->SaveLoadedScene();
}