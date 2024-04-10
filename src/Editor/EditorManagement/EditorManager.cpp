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

void EditorManager::InitializeEditor()
{	
	m_sWindowName = Application::GetApplication()->GetRenderer()->GetWindowTitle();

	BindEditorUI<MainMenuBar>("MainMenuBar");
	BindEditorUI<EditorUI>("Editor"); //this UI will probably be removed long term
	BindEditorUI<ResourceManagerWindow>("ResourceManager");
	BindEditorUI<Profiler>("Profiler");
	BindEditorUI<EditorSettingsWindow>("EditorSettings");
	BindEditorUI<ProjectDetailsWindow>("ProjectDetails");

	ToggleUI("MainMenuBar");
	ToggleUI("Editor");
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
	Renderer::UpdateWindowTitle(title);
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
}