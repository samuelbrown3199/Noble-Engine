#include "MeshRendererSystem.h"
#include "../Core/Application.h"
#include "../Core/Renderer.h"

#include <iostream>

std::weak_ptr<SystemBase> MeshRendererSystem::self;
std::weak_ptr<SystemBase> MeshRenderer::componentSystem;
std::vector<MeshRenderer> MeshRenderer::componentData;

void MeshRendererSystem::OnRender(MeshRenderer* comp)
{
	//Bind vertex memory buffer to our command buffer, then draw it.
	//VkBuffer vertexBuffers[] = { m_vertexBuffer };
	//VkDeviceSize offsets[] = { 0 };
	//vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

	//Command to draw our triangle. Parameters as follows, CommandBuffer, VertexCount, InstanceCount, firstVertex, firstInstance
	//vkCmdDraw(commandBuffer, static_cast<uint32_t>(vertices.size()), 1, 0, 0);
}