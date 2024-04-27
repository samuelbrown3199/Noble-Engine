#pragma once

#include <Engine/Core/Application.h>

#include "CommandSystem.h"

class ProjectFile;

class EditorManager : public Editor
{
private:

	std::string m_sDataFolder;
	std::unordered_map<std::string, std::shared_ptr<ToolUI>> m_mEditorUIs;
	std::deque<std::string> m_recentProjects;

	CommandSystem* m_pCommandSystem;

	void InitializeEditor();
	void CheckAndInitializeData();

	template<typename T>
	std::shared_ptr<T> BindEditorUI(std::string ID, ImGuiWindowFlags defaultFlags)
	{
		if (m_mEditorUIs.count(ID) != 0)
			LogFatalError(FormatString("Trying to bind UI %s that already exists.", ID.c_str()));

		std::shared_ptr<T> sys = std::make_shared<T>();
		sys->m_pEditor = this;
		sys->InitializeInterface(defaultFlags);
		m_mEditorUIs[ID] = sys;
		return sys;
	}

	void UpdateRecentProjectFile();

public:

	std::string m_sWindowName;
	bool m_bShowImGuiDemos = false;

	EditorManager();

	void ToggleUI(std::string ID);
	void CloseUI(std::string ID);
	void OpenUI(std::string ID);

	ToolUI* GetEditorUI(std::string ID);

	void UpdateEditorWindowTitle();

	void LoadScene(int sceneIndex);

	void OnUpdate() override;
	void OnRender() override;

	void HandleQuit() override;

	void SetProjectFile(ProjectFile* projectFile) override;
	void UpdateRecentProjects(ProjectFile* projectFile);
	std::deque<std::string> GetRecentProjects() { return m_recentProjects; }
	void RemoveRecentProject(int index);

	void PushCommand(Command* command) override;
	CommandSystem* GetCommandSystem() { return m_pCommandSystem; }

	void ConsoleLog(std::string message) override;
};