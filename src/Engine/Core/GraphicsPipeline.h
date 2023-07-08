#pragma once
#ifndef GRAPHICSPIPELINE_H_
#define GRAPHICSPIPELING_H_

#include <vector>
#include <string>

#include <vulkan/vulkan.h>

class GraphicsPipeline
{
	static std::vector<char> ReadShader(const std::string& filename);
	VkShaderModule CreateShaderModule(const std::vector<char>& code);

public:
	void CreatePipeline();
};

#endif