#pragma once
#ifndef EDITOR_H_
#define EDITOR_H_

#include <Engine\Core\DebugUI.hpp>
#include <Engine\ECS\Entity.hpp>

#include "ResourceManagerWindow.h"

class ProjectFile;

class EditorUI : public DebugUI
{
	ProjectFile* m_projectFile;

	bool show_demo_window = false;
	static int m_iSelEntity;
	static int m_iSelSystem;

	bool m_bOpenNewProj = false;

	static Entity* m_DebugCam;

	std::string m_sWindowName;

	std::shared_ptr<ResourceManagerWindow> m_resourceManagerWind;

	void CreateEditorCam();
	void UpdateEditorWindowTitle();

	void OpenResourceManager();

public:

	void InitializeInterface() override;
	void DoInterface() override;

	void HandleShortcutInputs();

	void DoMainMenuBar();
	void DoFileMenu();
	void DoAssetMenu();
	void DoToolMenu();
	void DoDebugMenu();

	void DoNewProjectModal();
};

#endif