#pragma once

#include <Engine\Core\DebugUI.hpp>
#include <Engine/Core/ResourceManager.h>

class ResourceManagerWindow : public DebugUI
{
private:

	std::shared_ptr<Resource> selResource = nullptr;

public:
	void InitializeInterface() override;
	void DoInterface() override;
};