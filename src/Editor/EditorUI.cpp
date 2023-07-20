#include "EditorUI.h"

#include <Engine\Useful.h>
#include <Engine\Core\Application.h>
#include <Engine\Core\Graphics\Renderer.h>
#include <Engine\Core\SceneManager.h>
#include <Engine\Behaviours\DebugCam.h>

int EditorUI::m_iSelEntity = -1;
int EditorUI::m_iSelSystem = -1;

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
	std::vector<std::shared_ptr<SystemBase>> systemList = Application::GetSystemList();
	if(ImGui::Button("Create Entity"))
	{
		Application::CreateEntity();
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete Entity"))
	{
		Application::DeleteEntity(entities.at(m_iSelEntity).m_sEntityID);
	}

	if (ImGui::TreeNode("Entities"))
	{
		static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
		static int selection_mask = (1 << 2);
		for (int i = 0; i < entities.size(); i++)
		{
			if (entities.at(i).m_bAvailableForUse)
				continue;

			// Disable the default "open on single-click behavior" + set Selected flag according to our selection.
			// To alter selection we use IsItemClicked() && !IsItemToggledOpen(), so clicking on an arrow doesn't alter selection.
			ImGuiTreeNodeFlags node_flags = base_flags;
			const bool is_selected = i == m_iSelEntity;
			if (is_selected)
				node_flags |= ImGuiTreeNodeFlags_Selected;

			bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, entities.at(i).m_sEntityName.c_str());
			if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
				m_iSelEntity = i;

			if (node_open)
			{
				for (int o = 0; o < systemList.size(); o++)
				{
					if (systemList.at(o)->GetComponentIndex(entities.at(i).m_sEntityID) != -1)
					{
						ImGui::Indent();
						ImGui::Selectable(systemList.at(o)->m_systemID.c_str());
						ImGui::Unindent();
					}
				}

				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}

	ImGui::Dummy(ImVec2(0.0f, 20.0f));

	if (ImGui::TreeNode("Systems"))
	{
		static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
		static int selection_mask = (1 << 2);
		for (int i = 0; i < systemList.size(); i++)
		{

			// Disable the default "open on single-click behavior" + set Selected flag according to our selection.
			// To alter selection we use IsItemClicked() && !IsItemToggledOpen(), so clicking on an arrow doesn't alter selection.
			ImGuiTreeNodeFlags node_flags = base_flags;
			const bool is_selected = i == m_iSelSystem;
			if (is_selected)
				node_flags |= ImGuiTreeNodeFlags_Selected;

			bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, systemList.at(i)->m_systemID.c_str());
			if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
				m_iSelSystem = i;

			if (node_open)
			{
				/*for (int o = 0; o < systemList.at(i); o++)
				{
					if (systemList.at(o)->GetComponentIndex(entities.at(i).m_sEntityID) != -1)
					{
						ImGui::Indent();
						ImGui::Selectable(systemList.at(o)->m_systemID.c_str());
						ImGui::Unindent();
					}
				}*/

				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
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

				Entity* entity = Application::CreateEntity();
				entity->AddBehaviour<DebugCam>();
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
