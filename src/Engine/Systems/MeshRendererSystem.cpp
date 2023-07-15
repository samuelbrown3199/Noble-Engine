#include "MeshRendererSystem.h"
#include "../Core/Application.h"
#include "../Core/Graphics/Renderer.h"
#include "../Core/Graphics/BufferHelper.h"
#include "../Core/Graphics/GraphicsPipeline.h"


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

	if (comp->m_model == nullptr)
		return;

	if (!comp->m_bCreatedDescriptorSets)
	{
		BufferHelper::CreateUniformBuffers(comp->m_uniformBuffers, comp->m_uniformBuffersMapped);
		Renderer::GetGraphicsPipeline()->CreateDescriptorSets(comp->m_descriptorSets, comp->m_uniformBuffers, comp->m_texture);

		comp->m_bCreatedDescriptorSets = true;
	}

	UniformBufferObject ubo{};
	ubo.model = comp->m_transform->m_transformMat; //This is worth moving to a push descriptor I think.
	ubo.view = Renderer::GenerateViewMatrix(); //Probably worth having UBO for Projection and View, bound once per frame.
	ubo.proj = Renderer::GenerateProjMatrix();

	memcpy(comp->m_uniformBuffersMapped[Renderer::GetCurrentFrame()], &ubo, sizeof(ubo)); //Not the most efficient way to do this, refer back to conclusion of https://vulkan-tutorial.com/Uniform_buffers/Descriptor_layout_and_buffer

	//Bind vertex memory buffer to our command buffer, then draw it.
	VkBuffer vertexBuffers[] = { comp->m_model->m_vertexBuffer.m_buffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(Renderer::GetCurrentCommandBuffer(), 0, 1, vertexBuffers, offsets);

	vkCmdBindIndexBuffer(Renderer::GetCurrentCommandBuffer(), comp->m_model->m_indexBuffer.m_buffer, 0, VK_INDEX_TYPE_UINT32);

	vkCmdBindDescriptorSets(Renderer::GetCurrentCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, Renderer::GetGraphicsPipeline()->GetPipelineLayout(), 0, 1, &comp->m_descriptorSets[Renderer::GetCurrentFrame()], 0, nullptr);
	//Command to draw our triangle. Parameters as follows, CommandBuffer, IndexCount, InstanceCount, firstIndex, vertexOffset, firstInstance
	vkCmdDrawIndexed(Renderer::GetCurrentCommandBuffer(), static_cast<uint32_t>(comp->m_model->m_indices.size()), 1, 0, 0, 0);
}