#pragma once

#include <Engine\Core\Registry.h>
#include <Engine\Core\ToolUI.hpp>
#include <Engine\Core\ECS\Entity.hpp>
#include <Engine\Core\ECS\Component.h>

#include "ResourceManagerWindow.h"
#include "Profiler.h"

class ProjectFile;

class EditorUI : public EditorToolUI
{
	static int m_iSelEntity;
	static int m_iSelSystem;

	Component* m_selComponent;
	static Entity* m_DebugCam;

	void ChangeEditorMode();
	void CreateEditorCam();

	ImVec4 ambientColour;
	ImVec4 clearColour;
	ImVec4 sunlightColour;

public:

	void InitializeInterface() override;
	void DoInterface() override;

	void HandleShortcutInputs();
};