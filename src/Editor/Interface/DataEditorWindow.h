#pragma once

#include <memory>

#include <Engine/Core/ToolUI.h>
#include <Engine/Core/ResourceManager.h>

class DataEditorWindow : public EditorToolUI
{
private:

	int m_iSelEntity = -1;
	std::shared_ptr<Resource> m_pSelResource = nullptr;

public:

	void DoInterface() override;

	void SetSelectedEntity(int _index);
	void ResetSelectedEntity() { m_iSelEntity = -1; }
	void SetSelectedResource(std::shared_ptr<Resource> _resource);
};