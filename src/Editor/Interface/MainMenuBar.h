#pragma once

#include <Engine\Core\ToolUI.hpp>

class NewProjectModal : public ToolModalWindow
{
public:

	void DoModal() override;
};

class QuitWarningModal : public ToolModalWindow
{
public:

	void DoModal() override;
};

class MainMenuBar : public EditorToolUI
{
	friend class NewProjectModal;

private:

	void DoMainMenuBar();

	void DoEditorMenu();
	void DoSceneMenu();
	void DoAssetMenu();
	void DoToolMenu();
	void DoDebugMenu();

public:

	void InitializeInterface() override;
	void DoInterface() override;
};