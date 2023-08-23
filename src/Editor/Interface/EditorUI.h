#pragma once

#include <Engine\Core\Registry.h>
#include <Engine\Core\DebugUI.hpp>
#include <Engine\ECS\Entity.hpp>
#include <Engine\ECS\Component.hpp>

#include "ResourceManagerWindow.h"

class ProjectFile;

class EditorUI : public DebugUI
{
	ProjectFile* m_projectFile;

	bool show_demo_window = false;
	static int m_iSelEntity;
	static int m_iSelSystem;

	Component* m_selComponent;

	bool m_bOpenNewProj = false;

	static Entity* m_DebugCam;

	std::string m_sWindowName;

	std::shared_ptr<ResourceManagerWindow> m_resourceManagerWind;

	void ChangeEditorMode();

	void CreateEditorCam();
	void UpdateEditorWindowTitle();

	void OpenResourceManager();

	ImVec4 ambientColour;
	float ambientStrength;
	ImVec4 clearColour;
	void UpdateLightingWidgets();

	void LoadScene(std::string scenePath);

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