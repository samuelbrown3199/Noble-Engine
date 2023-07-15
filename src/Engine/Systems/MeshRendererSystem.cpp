#include "MeshRendererSystem.h"
#include "../Core/Application.h"
#include "../Core/Graphics/Renderer.h"

#include <iostream>

std::weak_ptr<SystemBase> MeshRendererSystem::self;
std::weak_ptr<SystemBase> MeshRenderer::componentSystem;
std::vector<MeshRenderer> MeshRenderer::componentData;

void MeshRendererSystem::OnRender(MeshRenderer* comp)
{
	if (comp->m_transform == nullptr || Application::GetEntitiesDeleted())
	{
		comp->m_transform = Transform::GetComponent(comp->m_sEntityID);
	}

	//Bind vertex memory buffer to our command buffer, then draw it.
	/*VkBuffer vertexBuffers[] = {Renderer::GetVertexBuffer()};
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(Renderer::GetCurrentCommandBuffer(), 0, 1, vertexBuffers, offsets);

	vkCmdBindIndexBuffer(Renderer::GetCurrentCommandBuffer(), Renderer::GetIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

	//Command to draw our triangle. Parameters as follows, CommandBuffer, IndexCount, InstanceCount, firstIndex, vertexOffset, firstInstance
	vkCmdDrawIndexed(Renderer::GetCurrentCommandBuffer(), static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);*/
}