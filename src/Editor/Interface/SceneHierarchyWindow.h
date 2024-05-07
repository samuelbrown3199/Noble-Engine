#pragma once

#include <Engine/Core/ToolUI.h>

class CreateEntityModal : public ToolModalWindow
{

	std::string m_sEntityName = "New Entity";

	void CreateEntity();

public:

	void DoModal() override;

};

class SceneHierarchyWindow : public EditorToolUI
{
private:

	std::string m_sSelEntity = "";

public:

	void InitializeInterface(ImGuiWindowFlags defaultFlags) override;
	void DoInterface() override;

	void SetSelectedEntity(std::string ID);
	void ResetSelectedEntity() { m_sSelEntity = ""; }
};