#pragma once

#include <Engine\Core\ToolUI.hpp>

class EditorSettingsWindow : public EditorToolUI
{
public:

	void InitializeInterface(ImGuiWindowFlags defaultFlags) override;
	void DoInterface() override;
};