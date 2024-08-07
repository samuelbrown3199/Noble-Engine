#pragma once 

#include <vulkan/vulkan.h>

namespace vkutil {

	void TransitionImage(VkCommandBuffer cmd, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout);
	void CopyImageToImage(VkCommandBuffer cmd, VkImage source, VkImage destination, VkExtent2D srcSize, VkExtent2D dstSize, VkFilter filter = VK_FILTER_LINEAR);
	void GenerateMipmaps(VkCommandBuffer cmd, VkImage image, VkExtent2D imageSize);

};