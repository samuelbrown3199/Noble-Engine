#pragma once

#include <Engine\Core\ToolUI.h>

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

class AboutNobleModal : public ToolModalWindow
{
public:
	void DoModal() override;
};

class ProjectDoesntExistModal : public ToolModalWindow
{
public:

	int m_selectedProjectIndex = -1;

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
	void DoHelpMenu();
	void DoDebugMenu();

	void DoSecondaryMenuBar();

public:

	void InitializeInterface(ImGuiWindowFlags defaultFlags) override;
	void DoInterface() override;

	virtual void HandleShortcutInputs() override;
};