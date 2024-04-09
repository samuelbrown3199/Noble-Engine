#include "MainMenuBar.h"

#include <Engine\Useful.h>
#include <Engine\Core\Application.h>
#include <Engine\Core\Graphics\Renderer.h>
#include <Engine\Core\SceneManager.h>
#include <Engine\Core\EngineBehaviours\DebugCam.h>
#include <Engine\Core\InputManager.h>
#include <Engine\Useful.h>

#include "../EditorManagement/EditorManager.h"
#include "../EditorManagement/ProjectFile.h"

void NewProjectModal::DoModal()
{
	MainMenuBar* mainmenuBar = dynamic_cast<MainMenuBar*>(m_pParentUI);
	EditorManager* editorManager = dynamic_cast<EditorManager*>(mainmenuBar->m_pEditor);

	CheckIfToggled();

	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	if (ImGui::BeginPopupModal("New Project", NULL, ImGuiWindowFlags_AlwaysAutoResize))
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

void QuitWarningModal::DoModal()
{
	MainMenuBar* mainmenuBar = dynamic_cast<MainMenuBar*>(m_pParentUI);

	CheckIfToggled();

	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	if (ImGui::BeginPopupModal("Quit Noble Editor?", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
	{
		ImGui::Text("Are you sure you want to quit Noble Editor?\n\nAny unsaved work will be lost!");
		ImGui::Dummy(ImVec2(0.0f, 10.0f));

		if (ImGui::Button("Yes"))
		{
			Application::ForceQuit();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void AboutNobleModal::DoModal()
{
	MainMenuBar* mainmenuBar = dynamic_cast<MainMenuBar*>(m_pParentUI);

	CheckIfToggled();

	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	if (ImGui::BeginPopupModal("About Noble Engine", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
	{
		std::string versionString = GetVersionInfoString();
		std::string aboutString = "Noble Engine\n\nVersion: " + versionString + "\n\nDeveloped by : Samuel Brown";
		ImGui::Text(aboutString.c_str());
		ImGui::Dummy(ImVec2(0.0f, 10.0f));

		if (ImGui::Button("Close"))
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

		if (ImGui::BeginMenu("Editor"))
		{
			DoEditorMenu();
			ImGui::EndMenu();
		}

		if (editorManager->m_projectFile)
		{
			if (ImGui::BeginMenu("Scenes"))
			{
				DoSceneMenu();
				ImGui::EndMenu();
			}
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

		if (ImGui::BeginMenu("Help"))
		{
			DoHelpMenu();
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

void MainMenuBar::DoEditorMenu()
{
	EditorManager* editorManager = dynamic_cast<EditorManager*>(m_pEditor);

	ImGui::MenuItem("Project", NULL, false, false);
	if (ImGui::MenuItem("New Project"))
	{
		DoModal("New Project");
	}
	ImGui::SetItemTooltip("Create a new project.");

	if (ImGui::BeginMenu("Load Project"))
	{
		std::vector<std::string> projects = GetAllFilesOfType(GetWorkingDirectory() + "\\Projects", ".npj");
		for (int n = 0; n < projects.size(); n++)
		{
			if (ImGui::Button(projects.at(n).c_str()))
			{
				ProjectFile* projectFile = new ProjectFile();
				projectFile->LoadProjectFile(projects.at(n));

				editorManager->SetProjectFile(projectFile);
			}
		}

		ImGui::EndMenu();
	}
	ImGui::SetItemTooltip("Load an existing project.");
	if (editorManager->m_projectFile)
	{
		if (ImGui::MenuItem("Project Details"))
		{
			editorManager->ToggleUI("ProjectDetails");
		}
		ImGui::SetItemTooltip("View and change project details. NYI");
	}
	else
		ImGui::MenuItem("Project Details", NULL, false, false);

	ImGui::Dummy(ImVec2(0.0f, 5.0f));
	ImGui::MenuItem("Settings", NULL, false, false);
	if (ImGui::MenuItem("Editor Settings"))
	{
		editorManager->ToggleUI("EditorSettings");
	}
	ImGui::SetItemTooltip("Open the Settings Interface.");
	if (ImGui::MenuItem("Quit Editor"))
	{
		DoModal("Quit Noble Editor?");
	}
	ImGui::SetItemTooltip("Quit the Noble Editor.");
}

void MainMenuBar::DoSceneMenu()
{
	EditorManager* editorManager = dynamic_cast<EditorManager*>(m_pEditor);
	ImGui::MenuItem("Scene", NULL, false, false);
	if (ImGui::MenuItem("New Scene"))
	{
		//m_selComponent = nullptr; something will be needed to be done with this.
		SceneManager::ClearLoadedScene();
	}
	ImGui::SetItemTooltip("Create a new Scene.");

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
	ImGui::SetItemTooltip("Open an already existing scene from the project.");

	if (ImGui::MenuItem("Save Scene", "(CTRL+S)"))
	{
		if (Application::GetPlayMode())
		{
			return;
		}

		SceneManager::SaveLoadedScene();
	}
	ImGui::SetItemTooltip("Save the currently open scene.");

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
	ImGui::SetItemTooltip("Save the currently open scene as a new file.");
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

void MainMenuBar::DoHelpMenu()
{
	ImGui::MenuItem("Help", NULL, false, false);

	if (ImGui::MenuItem("About Noble Engine"))
	{
		DoModal("About Noble Engine");
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
	AddModal<NewProjectModal>("New Project");
	AddModal<QuitWarningModal>("Quit Noble Editor?");
	AddModal<AboutNobleModal>("About Noble Engine");
}

void MainMenuBar::DoInterface()
{
	DoMainMenuBar();
}