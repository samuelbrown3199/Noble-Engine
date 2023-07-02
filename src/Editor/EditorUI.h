#pragma once
#ifndef EDITOR_H_
#define EDITOR_H_

#include <Engine\Core\DebugUI.hpp>

class EditorUI : public DebugUI
{
	bool show_demo_window = false;
	static int m_iSelEntity;
	static int m_iSelSystem;

	std::string m_sWindowName;

public:

	void InitializeInterface();
	void DoInterface() override;
	void DoMainMenuBar();
	void DoFileMenu();
	void DoDebugMenu();
};

#endif