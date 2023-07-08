#pragma once
#ifndef GRAPHICSPIPELINE_H_
#define GRAPHICSPIPELING_H_

#include <vector>
#include <string>

#include <vulkan/vulkan.h>

class GraphicsPipeline
{
	VkRenderPass m_renderPass;
	VkPipelineLayout m_pipelineLayout;
	VkPipeline m_graphicsPipeline;

	static std::vector<char> ReadShader(const std::string& filename);
	VkShaderModule CreateShaderModule(const std::vector<char>& code);

	void CleanupPipeline();

	void CreateRenderPass();
	void CreatePipeline();

public:

	GraphicsPipeline();
	~GraphicsPipeline();
};

#endif