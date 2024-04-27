#pragma once

#include <Engine\Core\ToolUI.h>
#include <Engine\Core\ProjectFile.h>

class ProjectDetailsWindow : public EditorToolUI
{
private:

	ProjectFile* m_pProjectFile;

public:
	void InitializeInterface(ImGuiWindowFlags defaultFlags) override;
	void DoInterface() override;

	void SetProjectFile(ProjectFile* projectFile);
};