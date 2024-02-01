#pragma once 
#include "VulkanTypes.h"

namespace vkutil {

    bool LoadShaderModule(const char* filePath, VkDevice device, VkShaderModule* outShaderModule);

};