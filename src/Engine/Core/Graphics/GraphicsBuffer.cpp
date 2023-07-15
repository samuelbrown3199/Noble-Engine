#include "GraphicsBuffer.h"
#include "Renderer.h"

GraphicsBuffer::~GraphicsBuffer()
{
	vkDestroyBuffer(Renderer::GetLogicalDevice(), m_buffer, nullptr);
	vkFreeMemory(Renderer::GetLogicalDevice(), m_bufferMemory, nullptr);
}