#include "EditorManager.h"

#include <Engine\Core\Application.h>
#include <Engine\Core\Graphics\Renderer.h>
#include <Engine\Core\SceneManager.h>
#include <Engine\Core\EngineBehaviours\DebugCam.h>
#include <Engine\Core\InputManager.h>
#include <Engine\Core\ProjectFile.h>

#include "../Interface/MainMenuBar.h"
#include "../Interface/EditorUI.h"
#include "../Interface/EditorSettingsWindow.h"
#include "../Interface/ResourceManagerWindow.h"
#include "../Interface/Profiler.h"
#include "../Interface/ProjectDetailsWindow.h"
#include "../Interface/SceneManagerWindow.h"

void EditorManager::InitializeEditor()
{	
	m_sWindowName = Application::GetApplication()->GetRenderer()->GetWindowTitle();

	CheckAndInitializeData();

	BindEditorUI<MainMenuBar>("MainMenuBar", ImGuiWindowFlags_NoMove);
	BindEditorUI<EditorUI>("Editor", ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize); //this UI will probably be removed long term
	BindEditorUI<ResourceManagerWindow>("ResourceManager", ImGuiWindowFlags_AlwaysVerticalScrollbar);
	BindEditorUI<Profiler>("Profiler", ImGuiWindowFlags_None);
	BindEditorUI<EditorSettingsWindow>("EditorSettings", ImGuiWindowFlags_AlwaysAutoResize);
	BindEditorUI<ProjectDetailsWindow>("ProjectDetails", ImGuiWindowFlags_AlwaysAutoResize);
	BindEditorUI<SceneManagerWindow>("SceneManager", ImGuiWindowFlags_AlwaysAutoResize);

	ToggleUI("MainMenuBar");
	ToggleUI("Editor");
}

void EditorManager::CheckAndInitializeData()
{
	m_sDataFolder = GetWorkingDirectory() + "\\Data\\";
	if (!PathExists(m_sDataFolder))
	{
		CreateNewDirectory(m_sDataFolder);
		return;
	}

	if (PathExists(m_sDataFolder + "RecentProjects.txt"))
	{
		std::ifstream file(m_sDataFolder + "RecentProjects.txt");
		std::string line;
		while (std::getline(file >> std::ws, line))
		{
			m_recentProjects.push_back(line);
		}
		file.close();
	}
}

void EditorManager::ToggleUI(std::string ID)
{
	if (m_mEditorUIs.count(ID) == 0)
		Logger::LogError(FormatString("Trying to toggle UI %s that doesnt exist.", ID.c_str()), 2);

	m_mEditorUIs.at(ID)->m_uiOpen = !m_mEditorUIs.at(ID)->m_uiOpen;
}

void EditorManager::CloseUI(std::string ID)
{
	if (m_mEditorUIs.count(ID) == 0)
		Logger::LogError(FormatString("Trying to close UI %s that doesnt exist.", ID.c_str()), 2);

	m_mEditorUIs.at(ID)->m_uiOpen = false;
}

void EditorManager::OpenUI(std::string ID)
{
	if (m_mEditorUIs.count(ID) == 0)
		Logger::LogError(FormatString("Trying to open UI %s that doesnt exist.", ID.c_str()), 2);

	m_mEditorUIs.at(ID)->m_uiOpen = true;
}

ToolUI* EditorManager::GetEditorUI(std::string ID)
{
	if (m_mEditorUIs.count(ID) == 0)
		Logger::LogError(FormatString("Trying to get UI %s that doesnt exist.", ID.c_str()), 2);

	return m_mEditorUIs.at(ID).get();
}

EditorManager::EditorManager()
{
	InitializeEditor();
}

void EditorManager::UpdateEditorWindowTitle()
{
	std::string title = m_sWindowName;
	if (m_projectFile)
		title += "\t(Project " + m_projectFile->m_sProjectName + ")";

	title += "\t(" + Application::GetApplication()->GetSceneManager()->GetCurrentSceneLocalPath() + ")";
	Application::GetApplication()->GetRenderer()->UpdateWindowTitle(title);
}

void EditorManager::LoadScene(int sceneIndex)
{
	Application::GetApplication()->GetSceneManager()->LoadScene(sceneIndex);
	UpdateEditorWindowTitle();
}

void EditorManager::OnUpdate()
{
	std::map<std::string, std::shared_ptr<ToolUI>>::iterator uiItr;
	for (uiItr = m_mEditorUIs.begin(); uiItr != m_mEditorUIs.end(); uiItr++)
	{
		if (uiItr->second->m_uiOpen)
		{
			uiItr->second->UpdateWindowFlags();
			uiItr->second->DoInterface();
			uiItr->second->DoModals();
		}
	}

	if (m_bShowImGuiDemos)
	{
		ImGui::ShowDemoWindow();
		ImPlot::ShowDemoWindow();
	}
}

void EditorManager::OnRender() 
{
}

void EditorManager::HandleQuit()
{
	m_mEditorUIs["MainMenuBar"]->DoModal("Quit Noble Editor?");
}

void EditorManager::SetProjectFile(ProjectFile* projectFile)
{
	if (m_projectFile != nullptr)
		delete m_projectFile;

	m_projectFile = projectFile;
	dynamic_cast<ProjectDetailsWindow*>(m_mEditorUIs["ProjectDetails"].get())->SetProjectFile(projectFile);
	UpdateEditorWindowTitle();

	dynamic_cast<SceneManagerWindow*>(GetEditorUI("SceneManager"))->UpdateOriginalSceneOrder();
	UpdateRecentProjects(projectFile);
}

void EditorManager::UpdateRecentProjects(ProjectFile* projectFile)
{
	std::string path = projectFile->m_sProjectDirectory + "\\" + projectFile->m_sProjectName + ".npj";
	for (int i = 0; i < m_recentProjects.size(); i++)
	{
		if (m_recentProjects[i] == path)
		{
			m_recentProjects.erase(m_recentProjects.begin() + i);
			break;
		}
	}
	m_recentProjects.push_front(path);

	if(m_recentProjects.size() > 5)
		m_recentProjects.pop_back();

	UpdateRecentProjectFile();
}

void EditorManager::RemoveRecentProject(int index)
{
	m_recentProjects.erase(m_recentProjects.begin() + index);
	UpdateRecentProjectFile();
}

void EditorManager::UpdateRecentProjectFile()
{
	std::ofstream file(m_sDataFolder + "RecentProjects.txt", 'w');
	for (std::string project : m_recentProjects)
	{
		file << project << std::endl;
	}
	file.close();
}