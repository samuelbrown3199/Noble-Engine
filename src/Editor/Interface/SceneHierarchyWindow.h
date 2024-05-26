#pragma once

#include <Engine/Core/ToolUI.h>
#include <Engine/Core/CommandTypes.h>

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
	EntityCopy* m_pEntityCopy = nullptr;

public:

	void InitializeInterface(ImGuiWindowFlags defaultFlags) override;
	void DoInterface() override;

	virtual void HandleShortcutInputs() override;

	void SetSelectedEntity(std::string ID);
	void ResetSelectedEntity() { m_sSelEntity = ""; }

	void CreateEntityCopy(Entity* entity);
};