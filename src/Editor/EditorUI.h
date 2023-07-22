#pragma once
#ifndef EDITOR_H_
#define EDITOR_H_

#include <Engine\Core\DebugUI.hpp>
#include <Engine\ECS\Entity.hpp>

class EditorUI : public DebugUI
{
	bool show_demo_window = false;
	static int m_iSelEntity;
	static int m_iSelSystem;
	static Entity* m_DebugCam;

	std::string m_sWindowName;

	void CreateEditorCam();

public:

	void InitializeInterface();
	void DoInterface() override;

	void HandleShortcutInputs();

	void DoMainMenuBar();
	void DoFileMenu();
	void DoToolMenu();
	void DoDebugMenu();
};

#endif