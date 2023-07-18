#pragma once
#ifndef GRAPHICS_BUFFER_H_
#define GRAPHICS_BUFFER_H_

#include <vulkan/vulkan.h>

struct GraphicsBuffer
{
	VkBuffer m_buffer;
	VkDeviceMemory m_bufferMemory;
	uint32_t m_bufferSize;
	void* m_data = nullptr;

	~GraphicsBuffer();

	void Map();
	void WriteToBuffer(void* data);
	void UnMap();
};

#endif