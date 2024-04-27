#pragma once

#include <Engine/Core/ToolUI.h>
#include <vulkan/vulkan.h>

class SceneViewWindow : public EditorToolUI
{


public:

	VkDescriptorSet m_drawWindowSet = VK_NULL_HANDLE;

	void InitializeInterface(ImGuiWindowFlags defaultFlags) override;
	void DoInterface() override;
};