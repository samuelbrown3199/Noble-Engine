#include "EditorUI.h"

#include <Engine\Useful.h>
#include <Engine\Core\Application.h>
#include <Engine\Core\Renderer.h>
#include <Engine\Core\SceneManager.h>


int EditorUI::selEntity = 0;

void EditorUI::InitializeInterface()
{
	m_windowFlags |= ImGuiWindowFlags_NoMove;
	m_windowFlags |= ImGuiWindowFlags_NoTitleBar;
	m_windowFlags |= ImGuiWindowFlags_NoResize;

	m_sWindowName = Renderer::GetWindowTitle();
}

void EditorUI::DoInterface()
{
	DoMainMenuBar();

	ImGui::SetNextWindowPos(ImVec2(0, 20), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, Renderer::GetScreenSize().y));

	ImGui::Begin("Editor", &m_uiOpen, m_windowFlags);

	std::vector<Entity>& entities = Application::GetEntityList();
	if(ImGui::Button("Create Entity"))
	{
		Application::CreateEntity();
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete Entity"))
	{
		Application::DeleteEntity(entities.at(selEntity).m_sEntityID);
	}

	ImGui::Text("Entities");
	if (ImGui::BeginListBox("Entities", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
	{
		for (int i = 0; i < entities.size(); i++)
		{
			if (entities.at(i).m_bAvailableForUse)
				continue;

			const bool is_selected = (selEntity == i);
			if (ImGui::Selectable(entities.at(i).m_sEntityName.c_str(), is_selected))
				selEntity = i;

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndListBox();
	}

	ImGui::End();

	if (show_demo_window)
		ImGui::ShowDemoWindow();
}

void EditorUI::DoMainMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			DoFileMenu();
			ImGui::EndMenu();
		}

#ifndef NDEBUG
		if (ImGui::BeginMenu("Developer Debug"))
		{
			DoDebugMenu();
			ImGui::EndMenu();
		}
#endif

		ImGui::EndMainMenuBar();
	}
}

void EditorUI::DoFileMenu()
{
	ImGui::MenuItem("File", NULL, false, false);
	if (ImGui::MenuItem("New Scene"))
	{
		Application::ClearLoadedScene();
	}
	if (ImGui::BeginMenu("Open Scene"))
	{
		std::vector<std::string> scenes = GetAllFilesOfType(GetGameDataFolder(), ".nsc");
		for (int n = 0; n < scenes.size(); n++)
		{
			if (ImGui::Button(GetFolderLocationRelativeToGameData(scenes.at(n)).c_str()))
			{
				SceneManager::LoadScene(GetFolderLocationRelativeToGameData(scenes.at(n)));
				Renderer::UpdateWindowTitle(m_sWindowName + " (" + SceneManager::GetCurrentSceneLocalPath() + ")");
			}
		}

		ImGui::EndMenu();
	}
	if (ImGui::MenuItem("Save Scene"))
	{
		SceneManager::SaveLoadedScene();
	}
	if (ImGui::MenuItem("Save Scene As..."))
	{
		SceneManager::SaveScene("\\GameData\\Scenes\\SceneSaveAs.nsc");
	}

	ImGui::MenuItem("Settings", NULL, false, false);
	if (ImGui::MenuItem("Quit Editor"))
	{
		Application::StopApplication();
	}
}

void EditorUI::DoDebugMenu()
{
	ImGui::Checkbox("Demo Window", &show_demo_window);
}
