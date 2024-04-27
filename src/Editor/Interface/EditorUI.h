#pragma once

#include <Engine\Core\Registry.h>
#include <Engine\Core\ToolUI.h>
#include <Engine\Core\ECS\Entity.h>
#include <Engine\Core\ECS\Component.h>

#include "ResourceManagerWindow.h"
#include "Profiler.h"

class ProjectFile;

class EditorUI : public EditorToolUI
{
	void ChangeEditorMode();

	ImVec4 ambientColour;
	ImVec4 clearColour;
	ImVec4 sunlightColour;

public:

	void InitializeInterface(ImGuiWindowFlags defaultFlags) override;
	void DoInterface() override;

	void HandleShortcutInputs();
};