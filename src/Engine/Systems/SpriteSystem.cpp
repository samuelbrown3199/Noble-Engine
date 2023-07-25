#include "SpriteSystem.h"
#include "../Core/Application.h"
#include "../Core/Graphics/Renderer.h"
#include "../Core/Graphics/BufferHelper.h"
#include "../Core/Graphics/GraphicsPipeline.h"

#include <iostream>

std::weak_ptr<SystemBase> SpriteSystem::self;
std::weak_ptr<SystemBase> Sprite::componentSystem;
std::vector<Sprite> Sprite::componentData;

const std::vector<Vertex> vertices =
{
	{{-0.5f, -0.5f, 0.0}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
	{{0.5f, -0.5f, 0.0}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
	{{0.5f, 0.5f, 0.0}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
	{{-0.5f, 0.5f, 0.0}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
};

const std::vector<uint32_t> indices =
{
	0, 1, 2, 2, 3, 0
};

void SpriteSystem::PreRender()
{
	if (!m_bInitializedSpriteQuad)
	{
		BufferHelper::CreateVertexBuffer(m_vertexBuffer, vertices);
		BufferHelper::CreateIndexBuffer(m_indexBuffer, indices);

		m_bInitializedSpriteQuad = true;
	}
}

void SpriteSystem::OnRender(Sprite* comp)
{
	if (comp->m_spriteTransform == nullptr || Application::GetEntitiesDeleted())
	{
		comp->m_spriteTransform = Transform::GetComponent(comp->m_sEntityID);
		return;
	}
	if (comp->m_spriteTexture == nullptr)
		return;

	if (!comp->m_bCreatedDescriptorSets)
	{
		BufferHelper::CreateUniformBuffers(comp->m_uniformBuffers, comp->m_uniformBuffersMapped);
		Renderer::GetGraphicsPipeline()->CreateDescriptorSets(comp->m_descriptorSets, comp->m_uniformBuffers, comp->m_spriteTexture);

		comp->m_bCreatedDescriptorSets = true;
	}

	UniformBufferObject ubo{};
	ubo.model = comp->m_spriteTransform->m_transformMat; //This is worth moving to a push descriptor I think.
	ubo.view = Renderer::GenerateViewMatrix(); //Probably worth having UBO for Projection and View, bound once per frame.
	ubo.proj = Renderer::GenerateProjMatrix();

	memcpy(comp->m_uniformBuffersMapped[Renderer::GetCurrentFrame()], &ubo, sizeof(ubo)); //Not the most efficient way to do this, refer back to conclusion of https://vulkan-tutorial.com/Uniform_buffers/Descriptor_layout_and_buffer

	//Bind vertex memory buffer to our command buffer, then draw it.
	VkBuffer vertexBuffers[] = { m_vertexBuffer.m_buffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(Renderer::GetCurrentCommandBuffer(), 0, 1, vertexBuffers, offsets);

	vkCmdBindIndexBuffer(Renderer::GetCurrentCommandBuffer(), m_indexBuffer.m_buffer, 0, VK_INDEX_TYPE_UINT32);

	vkCmdBindDescriptorSets(Renderer::GetCurrentCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, Renderer::GetGraphicsPipeline()->GetPipelineLayout(), 0, 1, &comp->m_descriptorSets[Renderer::GetCurrentFrame()], 0, nullptr);
	vkCmdDrawIndexed(Renderer::GetCurrentCommandBuffer(), static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
}