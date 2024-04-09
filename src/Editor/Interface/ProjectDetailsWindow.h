#pragma once

#include <Engine\Core\ToolUI.hpp>

#include "../EditorManagement/ProjectFile.h"

class ProjectDetailsWindow : public EditorToolUI
{
private:

	ProjectFile* m_pProjectFile;

public:
	void InitializeInterface() override;
	void DoInterface() override;

	void SetProjectFile(ProjectFile* projectFile);
};