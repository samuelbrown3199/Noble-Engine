#pragma once

#include <memory>

#include <Engine/Core/ToolUI.h>
#include <Engine/Core/ResourceManager.h>

class DataEditorWindow : public EditorToolUI
{
private:

	std::string m_sSelEntity = "";
	std::shared_ptr<Resource> m_pSelResource = nullptr;

public:

	void DoInterface() override;

	void SetSelectedEntity(std::string ID);
	void ResetSelectedEntity() { m_sSelEntity = ""; }
	void SetSelectedResource(std::shared_ptr<Resource> _resource);
};