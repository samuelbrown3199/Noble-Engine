#pragma once
#ifndef GRAPHICS_BUFFER_H_
#define GRAPHICS_BUFFER_H_

#include <vulkan/vulkan.h>

struct GraphicsBuffer
{
	VkBuffer m_buffer;
	VkDeviceMemory m_bufferMemory;

	~GraphicsBuffer();
};

#endif