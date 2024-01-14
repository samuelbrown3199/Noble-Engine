#include "MeshRenderer.h"

#include "../Application.h"
#include "../Graphics/Renderer.h"
#include "../Graphics/BufferHelper.h"
#include "../Graphics/GraphicsPipeline.h"
#include "../ECS/Entity.hpp"

void MeshRenderer::OnRender() 
{
	if (m_model == nullptr)
		return;

	m_vertices = &m_model->m_vertices;
	m_indices = &m_model->m_indices;
	m_boundingBox = &m_model->m_modelBoundingBox;

	Renderable::OnRender();

	Transform* transform = NobleRegistry::GetComponent<Transform>(m_transformIndex);
	if (transform == nullptr)
		return;

	if (!m_bOnScreen)
		return;

	if (!m_bCreatedDescriptorSets)
	{
		BufferHelper::CreateUniformBuffers(m_uniformBuffers, m_uniformBuffersMapped);
		Renderer::GetGraphicsPipeline()->CreateDescriptorSets(m_descriptorSets, m_uniformBuffers, m_texture);

		m_bCreatedDescriptorSets = true;
	}

	UniformBufferObject ubo{};
	ubo.model = transform->m_transformMat; //This is worth moving to a push descriptor I think.
	ubo.view = Renderer::GenerateViewMatrix(); //Probably worth having UBO for Projection and View, bound once per frame.
	ubo.proj = Renderer::GenerateProjMatrix();

	memcpy(m_uniformBuffersMapped[Renderer::GetCurrentFrame()], &ubo, sizeof(ubo)); //Not the most efficient way to do this, refer back to conclusion of https://vulkan-tutorial.com/Uniform_buffers/Descriptor_layout_and_buffer

	//Bind vertex memory buffer to our command buffer, then draw it.
	VkBuffer vertexBuffers[] = { m_model->m_vertexBuffer.m_buffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(Renderer::GetCurrentCommandBuffer(), 0, 1, vertexBuffers, offsets);

	vkCmdBindIndexBuffer(Renderer::GetCurrentCommandBuffer(), m_model->m_indexBuffer.m_buffer, 0, VK_INDEX_TYPE_UINT32);

	vkCmdBindDescriptorSets(Renderer::GetCurrentCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, Renderer::GetGraphicsPipeline()->GetPipelineLayout(), 0, 1, &m_descriptorSets[Renderer::GetCurrentFrame()], 0, nullptr);
	//Command to draw our triangle. Parameters as follows, CommandBuffer, IndexCount, InstanceCount, firstIndex, vertexOffset, firstInstance
	vkCmdDrawIndexed(Renderer::GetCurrentCommandBuffer(), static_cast<uint32_t>(m_model->m_indices.size()), 1, 0, 0, 0);
}