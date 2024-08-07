#include "GraphicsBuffer.h"
#include "Renderer.h"

GraphicsBuffer::~GraphicsBuffer()
{
	if (m_bBufferExist)
	{
		vkDestroyBuffer(Renderer::GetLogicalDevice(), m_buffer, nullptr);
		vkFreeMemory(Renderer::GetLogicalDevice(), m_bufferMemory, nullptr);
	}

	m_bBufferExist = false;
}

void GraphicsBuffer::Map()
{
	vkMapMemory(Renderer::GetLogicalDevice(), m_bufferMemory, 0, m_bufferSize, 0, &m_data);
}

void GraphicsBuffer::WriteToBuffer(void* data)
{
	memcpy(m_data, data, (size_t)m_bufferSize);
}

void GraphicsBuffer::UnMap()
{
	vkUnmapMemory(Renderer::GetLogicalDevice(), m_bufferMemory);
}