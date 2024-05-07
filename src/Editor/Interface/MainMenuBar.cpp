#include "MainMenuBar.h"

#include <Engine\Useful.h>
#include <Engine\Core\Application.h>
#include <Engine\Core\Graphics\Renderer.h>
#include <Engine\Core\SceneManager.h>
#include <Engine\Core\InputManager.h>
#include <Engine\Useful.h>

#include <Engine\Core\ProjectFile.h>

#include "../EditorManagement/EditorManager.h"
#include "SceneHierarchyWindow.h"
#include "DataEditorWindow.h"

void NewProjectModal::DoModal()
{
	MainMenuBar* mainmenuBar = dynamic_cast<MainMenuBar*>(m_pParentUI);
	EditorManager* editorManager = dynamic_cast<EditorManager*>(mainmenuBar->m_pEditor);

	CheckIfToggled();

	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	if (ImGui::BeginPopupModal(m_sID.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		std::vector<std::string> templates = GetAllFilesOfType(GetWorkingDirectory() + "\\Templates", "", false);

		static char buf1[64] = ""; ImGui::InputText("Project Name", buf1, 64);
		static char buf2[256] = ""; ImGui::InputText("Project Directory", buf2, 256);

		static int selectedTemplate = -1;

		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		ImGui::Text("Select a template:");
		for (int i = 0; i < templates.size(); i++)
		{
			if (ImGui::Button(templates[i].c_str()))
			{
				selectedTemplate = i;
			}
		}

		if (ImGui::Button("Create"))
		{
			std::string projectFilePath = std::string(buf2) + "\\" + buf1 + ".npj";
			std::string projectGameData = std::string(buf2) + "\\GameData";

			if (selectedTemplate == -1)
			{
				return;
			}
			CopyFileToDestination(templates[selectedTemplate] + "\\Template.npj", projectFilePath);
			CopyDirectory(templates[selectedTemplate] + "\\GameData", projectGameData);

			ProjectFile* projectFile = new ProjectFile();
			projectFile->CreateProjectFile(buf1, buf2, projectFilePath);

			Application::GetApplication()->SetProjectFile(projectFilePath);

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
	if (ImGui::BeginPopupModal(m_sID.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
	{
		ImGui::Text("Are you sure you want to quit Noble Editor?\n\nAny unsaved work will be lost!");
		ImGui::Dummy(ImVec2(0.0f, 10.0f));

		if (ImGui::Button("Yes"))
		{
			Application::GetApplication()->ForceQuit();
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
	if (ImGui::BeginPopupModal(m_sID.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
	{
		std::string versionString = GetVersionInfoString();
		std::string aboutString = "Noble Engine\n\nVersion: " + versionString + "\n\nDeveloped by : Samuel Brown";
		aboutString += "\n\nNoble Engine is a game engine developed for the purpose of creating 2D and 3D games.";
		aboutString += "\nIt is currently in development and is not yet ready for commercial use.";
		aboutString += "\n\nCopyright (c) Samuel Brown 2024.";

		ImGui::Text(aboutString.c_str());
		ImGui::Dummy(ImVec2(0.0f, 10.0f));

		if (ImGui::Button("Close"))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void ProjectDoesntExistModal::DoModal()
{
	MainMenuBar* mainmenuBar = dynamic_cast<MainMenuBar*>(m_pParentUI);
	EditorManager* editorManager = dynamic_cast<EditorManager*>(mainmenuBar->m_pEditor);

	CheckIfToggled();

	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	if (ImGui::BeginPopupModal(m_sID.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
	{
		ImGui::Text("The project file doesn't exist. Please create a new project or load an existing one.");
		ImGui::Dummy(ImVec2(0.0f, 10.0f));

		if (ImGui::Button("Ok"))
		{
			editorManager->RemoveRecentProject(m_selectedProjectIndex);
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

		std::string playModeButton = "Enter Play Mode";
		if (Application::GetApplication()->GetPlayMode())
			playModeButton = "Enter Edit Mode";

		playModeButton += " temp button";
		if (ImGui::Button(playModeButton.c_str()))
		{
			editorManager->ChangeEditorMode();
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

	if (ImGui::Button("Load Project"))
	{
		IGFD::FileDialogConfig config;
		ImGuiFileDialog::Instance()->OpenDialog("ChooseProject", "Choose Project", ".npj", config);
	}
	ImGui::SetItemTooltip("Load an existing project.");

	if (ImGuiFileDialog::Instance()->Display("ChooseProject"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			std::string fileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
			std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
			Application::GetApplication()->SetProjectFile(filePath + "\\" + fileName);
		}
		ImGuiFileDialog::Instance()->Close();
	}

	if(ImGui::BeginMenu("Recent Projects"))
	{
		std::deque<std::string> recentProjects = editorManager->GetRecentProjects();
		for (int i = 0; i < recentProjects.size(); i++)
		{
			if (ImGui::MenuItem(recentProjects[i].c_str()))
			{
				if(PathExists(recentProjects[i]))
					Application::GetApplication()->SetProjectFile(recentProjects[i]);
				else
				{
					dynamic_cast<ProjectDoesntExistModal*>(GetModal("Project Doesn't Exist"))->m_selectedProjectIndex = i;
					DoModal("Project Doesn't Exist");
				}
			}
		}

		if (recentProjects.size() == 0)
			ImGui::MenuItem("No Recent Projects", NULL, false, false);

		ImGui::EndMenu();
	}
	ImGui::SetItemTooltip("Open a recently opened project.");

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
	if (ImGui::MenuItem("Scene Manager"))
	{
		editorManager->ToggleUI("SceneManager");
	}
	ImGui::SetItemTooltip("Opens the Scene Manager window.");

	if (ImGui::MenuItem("New Scene"))
	{
		dynamic_cast<SceneHierarchyWindow*>(editorManager->GetEditorUI("SceneHierarchy"))->ResetSelectedEntity();
		dynamic_cast<DataEditorWindow*>(editorManager->GetEditorUI("DataEditor"))->ResetSelectedEntity();
		Application::GetApplication()->GetSceneManager()->ClearLoadedScene();
	}
	ImGui::SetItemTooltip("Create a new Scene.");

	if (ImGui::BeginMenu("Open Scene"))
	{
		std::string path = GetGameDataFolder();
		std::vector<std::string>* scenes = Application::GetApplication()->GetSceneManager()->GetSceneList();
		for (int n = 0; n < scenes->size(); n++)
		{
			if (ImGui::Button(scenes->at(n).c_str()))
			{
				editorManager->LoadScene(n);
			}
		}

		ImGui::EndMenu();
	}
	ImGui::SetItemTooltip("Open an already existing scene from the project.");

	if (ImGui::MenuItem("Save Scene", "(CTRL+S)"))
	{
		if (Application::GetApplication()->GetPlayMode())
		{
			return;
		}

		Application::GetApplication()->GetSceneManager()->SaveLoadedScene();
	}
	ImGui::SetItemTooltip("Save the currently open scene.");

	if (ImGui::BeginMenu("Save Scene As..."))
	{
		if (Application::GetApplication()->GetPlayMode())
		{
			return;
		}

		static std::string newSceneName;
		ImGui::InputText("Scene Name", &newSceneName);

		ImGui::SameLine();
		if (ImGui::Button("Create"))
		{
			Application::GetApplication()->GetSceneManager()->CreateNewScene(newSceneName);
			newSceneName = "";
		}

		ImGui::EndMenu();
	}
	ImGui::SetItemTooltip("Save the currently open scene as a new file.");
}

void MainMenuBar::DoToolMenu()
{
	EditorManager* editorManager = dynamic_cast<EditorManager*>(m_pEditor);
	ImGui::MenuItem("Tools", NULL, false, false);
	if(ImGui::MenuItem("Undo", "(CTRL+Z)"))
	{
		editorManager->GetCommandSystem()->Undo();
	}
	if (ImGui::MenuItem("Redo", "(CTRL+Y)"))
	{
		editorManager->GetCommandSystem()->Redo();
	}
	if (ImGui::MenuItem("Editor Camera Settings TBD", ""))
	{
		//editorManager->ToggleUI("EditorCamSettings"); tbd
	}

	ImGui::Dummy(ImVec2(0.0f, 5.0f));

	ImGui::MenuItem("Windows", NULL, false, false);
	if (ImGui::MenuItem("Profiler", "(CTRL+P)"))
	{
		editorManager->ToggleUI("Profiler");
	}
	if (ImGui::MenuItem("Console"))
	{
		//editorManager->ToggleUI("Console");
	}
	if (ImGui::MenuItem("Scene View"))
	{
		editorManager->ToggleUI("SceneView");
	}
	if (ImGui::MenuItem("Data Editor"))
	{
		editorManager->ToggleUI("DataEditor");
	}
	if (ImGui::MenuItem("Scene Hierarchy"))
	{
		editorManager->ToggleUI("SceneHierarchy");
	}
	if (editorManager->m_projectFile != nullptr)
	{
		if(ImGui::MenuItem("Resource Manager", "(CTRL+R)"))
		{
			editorManager->ToggleUI("ResourceManager");
		}
	}
	//ImGui::Dummy(ImVec2(0.0f, 5.0f));
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

void MainMenuBar::InitializeInterface(ImGuiWindowFlags defaultFlags)
{
	EditorToolUI::InitializeInterface(defaultFlags);

	AddModal<NewProjectModal>("New Project");
	AddModal<QuitWarningModal>("Quit Noble Editor?");
	AddModal<AboutNobleModal>("About Noble Engine");
	AddModal<ProjectDoesntExistModal>("Project Doesn't Exist");
}

void MainMenuBar::DoInterface()
{
	DoMainMenuBar();
	HandleShortcutInputs();
}

void MainMenuBar::HandleShortcutInputs()
{
	EditorManager* editorManager = dynamic_cast<EditorManager*>(m_pEditor);
	InputManager* iManager = Application::GetApplication()->GetInputManager();
	CommandSystem* commandSystem = editorManager->GetCommandSystem();

	if (iManager->GetKeyDown(SDLK_r) && iManager->GetKey(SDLK_LCTRL))
	{
		if(editorManager->m_projectFile != nullptr)
			editorManager->ToggleUI("ResourceManager");
	}
	if (iManager->GetKeyDown(SDLK_p) && iManager->GetKey(SDLK_LCTRL))
	{
		editorManager->ToggleUI("Profiler");
	}

	if(iManager->GetKeyDown(SDLK_z) && iManager->GetKey(SDLK_LCTRL))
	{
		commandSystem->Undo();
	}
	if (iManager->GetKeyDown(SDLK_y) && iManager->GetKey(SDLK_LCTRL))
	{
		commandSystem->Redo();
	}
}