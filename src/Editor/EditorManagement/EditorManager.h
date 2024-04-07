#pragma once

#include <Engine/Core/Application.h>

class ProjectFile;

class EditorManager : public Editor
{
private:

	std::map<std::string, std::shared_ptr<ToolUI>> m_mEditorUIs;

	void InitializeEditor();

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

public:

	ProjectFile* m_projectFile = nullptr;
	std::string m_sWindowName;
	bool m_bShowImGuiDemos = false;

	EditorManager();

	void ToggleUI(std::string ID);
	void UpdateEditorWindowTitle();

	void LoadScene(std::string scenePath);

	void OnUpdate() override;
	void OnRender() override;

	void HandleQuit() override;
};