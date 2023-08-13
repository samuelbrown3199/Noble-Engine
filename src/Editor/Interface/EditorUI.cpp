#include "EditorUI.h"

#include <Engine\Useful.h>
#include <Engine\Core\Application.h>
#include <Engine\Core\Graphics\Renderer.h>
#include <Engine\Core\SceneManager.h>
#include <Engine\Behaviours\DebugCam.h>
#include <Engine\Core\InputManager.h>

#include <Engine\ECS\Entity.hpp>

#include <Engine\Resource\AudioClip.h>
#include <Engine\Resource\Model.h>
#include <Engine\Resource\Texture.h>
#include <Engine\Resource\Script.h>

#include "../EditorManagement/ProjectFile.h"

int EditorUI::m_iSelEntity = -1;
int EditorUI::m_iSelSystem = -1;

Entity* EditorUI::m_DebugCam = nullptr;

void EditorUI::CreateEditorCam()
{
	if (!m_DebugCam)
	{
		m_DebugCam = Application::CreateEntity();
		m_DebugCam->m_sEntityName = "Editor Cam";
		m_DebugCam->AddBehaviour<DebugCam>();
	}
	else
	{
		Application::DeleteEntity(m_DebugCam->m_sEntityID);
		m_DebugCam = nullptr;
	}
}

void EditorUI::UpdateEditorWindowTitle()
{
	std::string title = m_sWindowName;
	if (m_projectFile)
		title += "\t(Project " + m_projectFile->m_sProjectName + ")";
	
	title += "\t(" + SceneManager::GetCurrentSceneLocalPath() + ")";
	Renderer::UpdateWindowTitle(title);
}

void EditorUI::OpenResourceManager()
{
	if (!m_projectFile)
		return;

	m_resourceManagerWind->m_uiOpen = !m_resourceManagerWind->m_uiOpen;
}

void EditorUI::InitializeInterface()
{
	m_windowFlags |= ImGuiWindowFlags_NoMove;
	m_windowFlags |= ImGuiWindowFlags_NoTitleBar;
	m_windowFlags |= ImGuiWindowFlags_NoResize;

	m_sWindowName = Renderer::GetWindowTitle();

	m_resourceManagerWind = Application::BindDebugUI<ResourceManagerWindow>();
}

void EditorUI::DoInterface()
{
	HandleShortcutInputs();

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
		Application::DeleteEntity(entities.at(m_iSelEntity).m_sEntityID);
	}

	if (ImGui::TreeNode("Entities"))
	{
		std::map<int, std::pair<std::string, ComponentRegistry>>* compRegistry = NobleRegistry::GetComponentRegistry();

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
				for (int o = 0; o < compRegistry->size(); o++)
				{
					Component* comp = compRegistry->at(o).second.m_comp->GetAsComponent(entities.at(i).m_sEntityID);

					if (comp != nullptr)
					{
						ImGui::Indent();
						if (ImGui::Button(compRegistry->at(o).first.c_str()))
						{
							m_selComponent = comp;
						}
						ImGui::Unindent();
					}
				}

				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}

	ImGui::Dummy(ImVec2(0.0f, 10.0f));
	ImGui::SeparatorText("Component Information");
	ImGui::BeginChild("Component Info", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y/3), false);

	if (m_selComponent != nullptr )
		m_selComponent->DoComponentInterface();

	ImGui::EndChild();

	ImGui::Dummy(ImVec2(0.0f, 20.0f));
	static ImVec4 color = ImVec4(Renderer::GetClearColour().x, Renderer::GetClearColour().y, Renderer::GetClearColour().z, 200.0f / 255.0f);
	ImGui::ColorEdit3("Clear Colour", (float*)&color);
	if(ImGui::Button("Apply Clear Colour"))
		Renderer::SetClearColour(glm::vec3(color.x, color.y, color.z));

	ImGui::End();

	if (show_demo_window)
		ImGui::ShowDemoWindow();
}


void EditorUI::HandleShortcutInputs()
{
	if (InputManager::GetKey(SDLK_LCTRL) && InputManager::GetKeyDown(SDLK_q))
		CreateEditorCam();

	if (InputManager::GetKey(SDLK_LCTRL) && InputManager::GetKeyDown(SDLK_r))
		OpenResourceManager();
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
		DoNewProjectModal();

		if (m_projectFile)
		{
			if (ImGui::BeginMenu("Resources"))
			{
				DoAssetMenu();
				ImGui::EndMenu();
			}
		}

		if (ImGui::BeginMenu("Tools"))
		{
			DoToolMenu();
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
	ImGui::MenuItem("Project", NULL, false, false);
	if (ImGui::MenuItem("New Project"))
	{
		m_bOpenNewProj = true;
	}
	if (ImGui::BeginMenu("Load Project"))
	{
		std::vector<std::string> projects = GetAllFilesOfType(GetWorkingDirectory() + "\\Projects", ".npj");
		for (int n = 0; n < projects.size(); n++)
		{
			if (ImGui::Button(projects.at(n).c_str()))
			{
				if (m_projectFile == nullptr)
					m_projectFile = new ProjectFile();

				m_projectFile->LoadProjectFile(projects.at(n));
				UpdateEditorWindowTitle();
			}
		}

		ImGui::EndMenu();
	}

	ImGui::MenuItem("Scene", NULL, false, false);
	if (m_projectFile)
	{
		if (ImGui::MenuItem("New Scene"))
		{
			m_selComponent = nullptr;
			Application::ClearLoadedScene();
		}
		if (ImGui::BeginMenu("Open Scene"))
		{
			std::string path = GetGameDataFolder();
			std::vector<std::string> scenes = GetAllFilesOfType(path, ".nsc");
			for (int n = 0; n < scenes.size(); n++)
			{
				if (ImGui::Button(GetFolderLocationRelativeToGameData(scenes.at(n)).c_str()))
				{
					SceneManager::LoadScene(GetFolderLocationRelativeToGameData(scenes.at(n)));
					UpdateEditorWindowTitle();
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
	}
	else
	{
		ImGui::MenuItem("New Scene", NULL, false, false);
		ImGui::BeginMenu("Open Scene", false);
		ImGui::MenuItem("Save Scene", NULL, false, false);
		ImGui::MenuItem("Save Scene As...", NULL, false, false);
	}

	ImGui::MenuItem("Settings", NULL, false, false);
	if (ImGui::MenuItem("Quit Editor"))
	{
		Application::StopApplication();
	}
}

void EditorUI::DoAssetMenu()
{
	ImGui::MenuItem("Resources", NULL, false, false);
	if (ImGui::BeginMenu("Add Resource"))
	{
		std::map<int, std::pair<std::string, Resource*>>* resourceRegistry = NobleRegistry::GetResourceRegistry();

		for (int i = 0; i < resourceRegistry->size(); i++)
		{
			if (ImGui::MenuItem(resourceRegistry->at(i).first.c_str()))
			{
				std::string path = OpenFileSelectDialog(".mp3");
				if (path != "")
				{
					resourceRegistry->at(i).second->AddResource(path);
				}
			}
		}
		ImGui::EndMenu();
	}

	if (ImGui::MenuItem("Resource Manager", "(CTRL+R)"))
	{
		OpenResourceManager();
	}
}

void EditorUI::DoToolMenu()
{
	ImGui::MenuItem("Tools", NULL, false, false);

	std::string shortcut = "(Ctrl+Q)";
	std::string camButton = "Create Editor Cam";
	if(m_DebugCam != nullptr)
		camButton = "Delete Editor Cam";
	if (ImGui::MenuItem(camButton.c_str(), shortcut.c_str()))
	{
		CreateEditorCam();
	}
}

void EditorUI::DoDebugMenu()
{
	ImGui::Checkbox("Demo Window", &show_demo_window);
}

void EditorUI::DoNewProjectModal()
{
	if (m_bOpenNewProj)
	{
		ImGui::OpenPopup("New Project");
	}

	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(500.0f, 150.0f));
	if (ImGui::BeginPopupModal("New Project"))
	{
		static char buf1[64] = ""; ImGui::InputText("Project Name", buf1, 64);
		static char buf2[256] = ""; ImGui::InputText("Project Directory", buf2, 256);

		if (ImGui::Button("Create"))
		{
			ProjectFile::CreateProjectFile(buf1, buf2);

			if (m_projectFile == nullptr)
				m_projectFile = new ProjectFile();

			m_projectFile->LoadProjectFile(GetWorkingDirectory() + "\\Projects\\" + buf1 + ".npj");
			UpdateEditorWindowTitle();

			m_bOpenNewProj = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			m_bOpenNewProj = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}