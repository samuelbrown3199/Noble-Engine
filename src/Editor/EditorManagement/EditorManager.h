#pragma once

#include <Engine/Core/Application.h>

class ProjectFile;

class EditorManager : public Editor
{
private:

	std::string m_sDataFolder;
	std::map<std::string, std::shared_ptr<ToolUI>> m_mEditorUIs;
	std::deque<std::string> m_recentProjects;

	void InitializeEditor();
	void CheckAndInitializeData();

	template<typename T>
	std::shared_ptr<T> BindEditorUI(std::string ID)
	{
		if (m_mEditorUIs.count(ID) != 0)
			Logger::LogError(FormatString("Trying to bind UI %s that already exists.", ID.c_str()), 2);

		std::shared_ptr<T> sys = std::make_shared<T>();
		sys->m_pEditor = this;
		sys->InitializeInterface();
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
};