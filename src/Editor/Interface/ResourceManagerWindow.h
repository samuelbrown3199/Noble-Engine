#pragma once

#include <Engine\Core\DebugUI.hpp>
#include <Engine/Core/ResourceManager.h>

class ResourceManagerWindow : public DebugUI
{
private:

	std::shared_ptr<Resource> selResource = nullptr;

	bool m_bOpenNoFileResourceWindow = false;
	int m_iNewResourceType = 0;

public:
	void InitializeInterface() override;
	void DoInterface() override;
};