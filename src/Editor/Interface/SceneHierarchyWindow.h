#pragma once

#include <Engine/Core/ToolUI.h>

class SceneHierarchyWindow : public EditorToolUI
{
private:

	int m_iSelEntity = -1;

public:

	void DoInterface() override;
};