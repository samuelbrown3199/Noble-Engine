#include "EditorManager.h"

#include <Engine\Core\Application.h>
#include <Engine\Core\Graphics\Renderer.h>
#include <Engine\Core\SceneManager.h>
#include <Engine\Core\EngineBehaviours\DebugCam.h>
#include <Engine\Core\InputManager.h>

#include "../Interface/MainMenuBar.h"
#include "../Interface/EditorUI.h"
#include "ProjectFile.h"

void EditorManager::InitializeEditor()
{	
	m_sWindowName = Application::GetRenderer()->GetWindowTitle();

	BindEditorUI<MainMenuBar>("MainMenuBar");
	BindEditorUI<EditorUI>("Editor"); //this UI will probably be removed long term
	BindEditorUI<ResourceManagerWindow>("ResourceManager");
	BindEditorUI<Profiler>("Profiler");

	ToggleUI("MainMenuBar");
	ToggleUI("Editor");
}

void EditorManager::ToggleUI(std::string ID)
{
	if (m_mEditorUIs.count(ID) == 0)
		Logger::LogError(FormatString("Trying to open UI %s that doesnt exist.", ID.c_str()), 2);

	m_mEditorUIs.at(ID)->m_uiOpen = !m_mEditorUIs.at(ID)->m_uiOpen;
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

	title += "\t(" + SceneManager::GetCurrentSceneLocalPath() + ")";
	Renderer::UpdateWindowTitle(title);
}

void EditorManager::LoadScene(std::string scenePath)
{
	SceneManager::LoadScene(scenePath);

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