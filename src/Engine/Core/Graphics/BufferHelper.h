#pragma once
#ifndef BUFFERHELPER_H_
#define BUFFERHELPER_H_

#include <vulkan/vulkan.h>

#include "Renderer.h"
#include "../Logger.h"
#include "GraphicsBuffer.h"

class BufferHelper
{

public:

	static void CreateGraphicsBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, GraphicsBuffer& buffer);
	static uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	static void CreateVertexBuffer(GraphicsBuffer& buffer, std::vector<Vertex> vertices);
	static void CreateIndexBuffer(GraphicsBuffer& buffer, std::vector<uint32_t> indices);
	static void CopyGraphicsBuffer(GraphicsBuffer& srcBuffer, GraphicsBuffer& dstBuffer, VkDeviceSize size);
	static void CopyBufferToImage(GraphicsBuffer& buffer, VkImage image, uint32_t width, uint32_t height);
	static void CreateUniformBuffers(std::vector<GraphicsBuffer>& uniformBuffers, std::vector<void*>& uniformBuffersMapped);

private:

	static void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	static void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
};

#endif