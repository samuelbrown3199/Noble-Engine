#pragma once

#include <Engine/Core/ToolUI.h>

class DataEditorWindow : public EditorToolUI
{
private:

	int m_iSelEntity = -1;

public:

	void DoInterface() override;

	void SetSelectedEntity(int _index) { m_iSelEntity = _index; }
};