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

	int m_iSelEntity = -1;

public:

	void InitializeInterface(ImGuiWindowFlags defaultFlags) override;
	void DoInterface() override;

	void SetSelectedEntity(int iEntity);
	void ResetSelectedEntity() { m_iSelEntity = -1; }
};