#include "MainMenuBar.h"

#include <Engine\Useful.h>
#include <Engine\Core\Application.h>
#include <Engine\Core\Graphics\Renderer.h>
#include <Engine\Core\SceneManager.h>
#include <Engine\Core\EngineBehaviours\DebugCam.h>
#include <Engine\Core\InputManager.h>

#include "../EditorManagement/EditorManager.h"
#include "../EditorManagement/ProjectFile.h"

void NewProjectModal::DoModal()
{
	MainMenuBar* mainmenuBar = dynamic_cast<MainMenuBar*>(m_pParentUI);
	EditorManager* editorManager = dynamic_cast<EditorManager*>(mainmenuBar->m_pEditor);

	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(500.0f, 150.0f));
	if (ImGui::BeginPopupModal("NewProject"))
	{
		static char buf1[64] = ""; ImGui::InputText("Project Name", buf1, 64);
		static char buf2[256] = ""; ImGui::InputText("Project Directory", buf2, 256);

		if (ImGui::Button("Create"))
		{
			ProjectFile::CreateProjectFile(buf1, buf2);

			if (editorManager->m_projectFile == nullptr)
				editorManager->m_projectFile = new ProjectFile();

			editorManager->m_projectFile->LoadProjectFile(GetWorkingDirectory() + "\\Projects\\" + buf1 + ".npj");
			editorManager->UpdateEditorWindowTitle();

			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}


void MainMenuBar::DoMainMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		EditorManager* editorManager = dynamic_cast<EditorManager*>(m_pEditor);

		if (ImGui::BeginMenu("File"))
		{
			DoFileMenu();
			ImGui::EndMenu();
		}

		if (editorManager->m_projectFile)
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

		if (ImGui::BeginMenu("Debug"))
		{
			DoDebugMenu();
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

void MainMenuBar::DoFileMenu()
{
	EditorManager* editorManager = dynamic_cast<EditorManager*>(m_pEditor);

	ImGui::MenuItem("Project", NULL, false, false);
	if (ImGui::MenuItem("New Project"))
	{
		DoModal("NewProject");
	}

	if (ImGui::BeginMenu("Load Project"))
	{
		std::vector<std::string> projects = GetAllFilesOfType(GetWorkingDirectory() + "\\Projects", ".npj");
		for (int n = 0; n < projects.size(); n++)
		{
			if (ImGui::Button(projects.at(n).c_str()))
			{
				if (editorManager->m_projectFile == nullptr)
					editorManager->m_projectFile = new ProjectFile();

				editorManager->m_projectFile->LoadProjectFile(projects.at(n));
				editorManager->UpdateEditorWindowTitle();
			}
		}

		ImGui::EndMenu();
	}

	ImGui::MenuItem("Scene", NULL, false, false);
	if (editorManager->m_projectFile)
	{
		if (ImGui::MenuItem("New Scene"))
		{
			//m_selComponent = nullptr;
			SceneManager::ClearLoadedScene();
		}
		if (ImGui::BeginMenu("Open Scene"))
		{
			std::string path = GetGameDataFolder();
			std::vector<std::string> scenes = GetAllFilesOfType(path, ".nsc");
			for (int n = 0; n < scenes.size(); n++)
			{
				if (ImGui::Button(GetFolderLocationRelativeToGameData(scenes.at(n)).c_str()))
				{
					editorManager->LoadScene(GetFolderLocationRelativeToGameData(scenes.at(n)));
				}
			}

			ImGui::EndMenu();
		}

		if (ImGui::MenuItem("Save Scene", "(CTRL+S)"))
		{
			if (Application::GetPlayMode())
			{
				return;
			}

			SceneManager::SaveLoadedScene();
		}

		if (ImGui::BeginMenu("Save Scene As..."))
		{
			if (Application::GetPlayMode())
			{
				return;
			}

			std::string newSceneName;
			ImGui::InputText("Scene Name", &newSceneName);

			static std::string path = "";
			if (!newSceneName.empty())
			{
				path = ResourceManager::GetResourceManagerWorkingDirectory() + "\\GameData\\" + newSceneName + ".nsc";
			}

			ImGui::SameLine();
			if (ImGui::Button("Create"))
			{
				SceneManager::SaveScene(path);
			}

			ImGui::EndMenu();
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

void MainMenuBar::DoAssetMenu()
{
	EditorManager* editorManager = dynamic_cast<EditorManager*>(m_pEditor);

	ImGui::MenuItem("Resources", NULL, false, false);
	if (ImGui::BeginMenu("Add Resource"))
	{
		std::map<int, std::pair<std::string, ResourceRegistry>>* resourceRegistry = NobleRegistry::GetResourceRegistry();

		for (int i = 0; i < resourceRegistry->size(); i++)
		{
			if (ImGui::MenuItem(resourceRegistry->at(i).first.c_str()))
			{
				std::string path = OpenFileSelectDialog(".mp3");
				if (path != "")
				{
					resourceRegistry->at(i).second.m_resource->AddResource(path);
				}
			}
		}
		ImGui::EndMenu();
	}

	if (ImGui::MenuItem("Resource Manager", "(CTRL+R)"))
	{
		editorManager->ToggleUI("ResourceManager");
	}
}

void MainMenuBar::DoToolMenu()
{
	EditorManager* editorManager = dynamic_cast<EditorManager*>(m_pEditor);
	ImGui::MenuItem("Tools", NULL, false, false);

	if (ImGui::MenuItem("Profiler", "(CTRL+P)"))
	{
		editorManager->ToggleUI("Profiler");
	}
}

void MainMenuBar::DoDebugMenu()
{
	EditorManager* editorManager = dynamic_cast<EditorManager*>(m_pEditor);
	if (ImGui::MenuItem("ImGui Examples"))
	{
		editorManager->m_bShowImGuiDemos = !editorManager->m_bShowImGuiDemos;
	}
}

void MainMenuBar::InitializeInterface()
{
	AddModal<NewProjectModal>("NewProject");
}

void MainMenuBar::DoInterface()
{
	DoMainMenuBar();
}