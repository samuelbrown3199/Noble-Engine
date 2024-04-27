#pragma once

#include <Engine\Core\ToolUI.h>

class EditorSettingsWindow : public EditorToolUI
{
public:

	void InitializeInterface(ImGuiWindowFlags defaultFlags) override;
	void DoInterface() override;
};