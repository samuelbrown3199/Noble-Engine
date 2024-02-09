#include "Sprite.h"
#include "../Application.h"
#include "../Graphics/Renderer.h"
#include "../Graphics/BufferHelper.h"
#include "../Graphics/GraphicsPipeline.h"
#include "../ECS/Entity.hpp"

bool Sprite::m_bInitializedSpriteQuad = false;
//GraphicsBuffer Sprite::m_vertexBuffer;
//GraphicsBuffer Sprite::m_indexBuffer;

/*std::vector<Vertex> Sprite::vertices =
{
	{{0.0f, 0.0f, 0.0}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
	{{1.0f, 0.0f, 0.0}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
	{{1.0f, 1.0f, 0.0}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
	{{0.0f, 1.0f, 0.0}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
};*/

std::vector<glm::vec3> Sprite::boundingBox =
{
	glm::vec3(0.0f, 0.0f, 0.0),
	glm::vec3(1.0f, 0.0f, 0.0),
	glm::vec3(1.0f, 1.0f, 0.0),
	glm::vec3(0.0f, 1.0f, 0.0),
};

std::vector<uint32_t> Sprite::indices =
{
	0, 1, 2, 2, 3, 0
};

void Sprite::ClearSpriteBuffers()
{

}

void Sprite::PreRender()
{
	/*if (!m_bInitializedSpriteQuad)
	{
		BufferHelper::CreateVertexBuffer(m_vertexBuffer, vertices);
		BufferHelper::CreateIndexBuffer(m_indexBuffer, indices);

		m_bInitializedSpriteQuad = true;
	}*/
}

void Sprite::OnRender()
{
	/*m_vertices = &vertices;
	m_boundingBox = &boundingBox;
	m_indices = &indices;

	Renderable::OnRender();

	Transform* transform = NobleRegistry::GetComponent<Transform>(m_transformIndex);
	if (transform == nullptr)
		return;

	if (!m_bOnScreen)
		return;

	if (m_spriteTexture == nullptr)
		return;

	if (!m_bCreatedDescriptorSets)
	{
		BufferHelper::CreateUniformBuffers(m_uniformBuffers, m_uniformBuffersMapped);
		Renderer::GetGraphicsPipeline()->CreateDescriptorSets(m_descriptorSets, m_uniformBuffers, m_spriteTexture);

		m_bCreatedDescriptorSets = true;
	}

	UniformBufferObject ubo{};
	ubo.model = transform->m_transformMat; //This is worth moving to a push descriptor I think.
	ubo.view = Renderer::GenerateViewMatrix(); //Probably worth having UBO for Projection and View, bound once per frame.
	ubo.proj = Renderer::GenerateProjMatrix();

	memcpy(m_uniformBuffersMapped[Renderer::GetCurrentFrame()], &ubo, sizeof(ubo)); //Not the most efficient way to do this, refer back to conclusion of https://vulkan-tutorial.com/Uniform_buffers/Descriptor_layout_and_buffer

	//Bind vertex memory buffer to our command buffer, then draw it.
	VkBuffer vertexBuffers[] = { m_vertexBuffer.m_buffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(Renderer::GetCurrentCommandBuffer(), 0, 1, vertexBuffers, offsets);

	vkCmdBindIndexBuffer(Renderer::GetCurrentCommandBuffer(), m_indexBuffer.m_buffer, 0, VK_INDEX_TYPE_UINT32);

	vkCmdBindDescriptorSets(Renderer::GetCurrentCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, Renderer::GetGraphicsPipeline()->GetPipelineLayout(), 0, 1, &m_descriptorSets[Renderer::GetCurrentFrame()], 0, nullptr);
	vkCmdDrawIndexed(Renderer::GetCurrentCommandBuffer(), static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);*/
}