#include "BufferHelper.h"

void BufferHelper::CreateGraphicsBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, GraphicsBuffer& buffer)
{
	CreateBuffer(size, usage, properties, buffer.m_buffer, buffer.m_bufferMemory);
}

uint32_t BufferHelper::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(Renderer::GetPhysicalDevice(), &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}

	Logger::LogError("Failed to find suitable memory type.", 2);
}

void BufferHelper::CreateVertexBuffer(GraphicsBuffer& buffer, std::vector<Vertex> vertices)
{
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	GraphicsBuffer stagingBuffer;
	CreateGraphicsBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer);

	void* data;
	vkMapMemory(Renderer::GetLogicalDevice(), stagingBuffer.m_bufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(Renderer::GetLogicalDevice(), stagingBuffer.m_bufferMemory);

	CreateGraphicsBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer);
	CopyGraphicsBuffer(stagingBuffer, buffer, bufferSize);
}

void BufferHelper::CreateIndexBuffer(GraphicsBuffer& buffer, std::vector<uint32_t> indices)
{
	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	GraphicsBuffer stagingBuffer;
	CreateGraphicsBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer);

	void* data;
	vkMapMemory(Renderer::GetLogicalDevice(), stagingBuffer.m_bufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, indices.data(), (size_t)bufferSize);
	vkUnmapMemory(Renderer::GetLogicalDevice(), stagingBuffer.m_bufferMemory);

	CreateGraphicsBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer); //Uses VK_BUFFER_USAGE_INDEX_BUFFER_BIT instead of VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
	CopyGraphicsBuffer(stagingBuffer, buffer, bufferSize);
}

void BufferHelper::CopyGraphicsBuffer(GraphicsBuffer& srcBuffer, GraphicsBuffer& dstBuffer, VkDeviceSize size)
{
	CopyBuffer(srcBuffer.m_buffer, dstBuffer.m_buffer, size);
}

void BufferHelper::CopyBufferToImage(GraphicsBuffer& buffer, VkImage image, uint32_t width, uint32_t height)
{
	VkCommandBuffer commandBuffer = Renderer::BeginSingleTimeCommand();

	VkBufferImageCopy region{};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = {
		width,
		height,
		1
	};

	vkCmdCopyBufferToImage(
		commandBuffer,
		buffer.m_buffer,
		image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&region
	);

	Renderer::EndSingleTimeCommands(commandBuffer);
}

void BufferHelper::CreateUniformBuffers(std::vector<GraphicsBuffer>& uniformBuffers, std::vector<void*>& uniformBuffersMapped)
{
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	uniformBuffers.resize(Renderer::GetFrameCount());
	uniformBuffersMapped.resize(Renderer::GetFrameCount());

	for (size_t i = 0; i < Renderer::GetFrameCount(); i++)
	{
		BufferHelper::CreateGraphicsBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i]);

		vkMapMemory(Renderer::GetLogicalDevice(), uniformBuffers[i].m_bufferMemory, 0, bufferSize, 0, &uniformBuffersMapped[i]);
	}
}

void BufferHelper::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
	//Should maybe be a class that can be reused

	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(Renderer::GetLogicalDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
	{
		Logger::LogError("Failed to create vertex buffer.", 2);
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(Renderer::GetLogicalDevice(), buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	if (vkAllocateMemory(Renderer::GetLogicalDevice(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) //Refer back to the conclusion of https://vulkan-tutorial.com/Vertex_buffers/Staging_buffer, refers to limitations of allocating memory.
	{
		Logger::LogError("Failed to allocate vertex buffer memory!", 2);
	}

	vkBindBufferMemory(Renderer::GetLogicalDevice(), buffer, bufferMemory, 0);
}

void BufferHelper::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	VkCommandBuffer commandBuffer = Renderer::BeginSingleTimeCommand();

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0; // Optional
	copyRegion.dstOffset = 0; // Optional
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	Renderer::EndSingleTimeCommands(commandBuffer);
}