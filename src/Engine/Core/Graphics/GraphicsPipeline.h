#pragma once
#ifndef GRAPHICSPIPELINE_H_
#define GRAPHICSPIPELING_H_

#include <vector>
#include <string>

#include <vulkan/vulkan.h>

class GraphicsPipeline
{
	VkRenderPass m_renderPass;
	VkDescriptorSetLayout m_descriptorSetLayout;
	VkPipelineLayout m_pipelineLayout;
	VkPipeline m_graphicsPipeline;

	static std::vector<char> ReadShader(const std::string& filename);
	VkShaderModule CreateShaderModule(const std::vector<char>& code);

	void CleanupPipeline();

	void CreateDescriptorSetLayout();
	void CreateRenderPass();
	void CreatePipeline();

public:

	VkRenderPass GetRenderPass() { return m_renderPass; }
	VkPipeline GetGraphicsPipeline() { return m_graphicsPipeline; }
	VkPipelineLayout GetPipelineLayout() { return m_pipelineLayout; }

	VkDescriptorSetLayout GetDescriptorSetLayout() { return m_descriptorSetLayout; }

	GraphicsPipeline();
	~GraphicsPipeline();
};

#endif