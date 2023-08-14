#include "Sprite.h"
#include "../Core/Application.h"
#include "../Core/Graphics/Renderer.h"
#include "../Core/Graphics/BufferHelper.h"
#include "../Core/Graphics/GraphicsPipeline.h"
#include "../ECS/Entity.hpp"

bool Sprite::m_bInitializedSpriteQuad = false;
GraphicsBuffer Sprite::m_vertexBuffer;
GraphicsBuffer Sprite::m_indexBuffer;

ComponentDatalist<Sprite> Sprite::m_componentList;

void Sprite::AddComponent()
{
	m_componentList.AddComponent(this);
}

void Sprite::AddComponentToEntity(std::string entityID)
{
	m_componentList.AddComponentToEntity(entityID);
	Application::GetEntity(entityID)->GetAllComponents();
}

void Sprite::RemoveComponent(std::string entityID)
{
	m_componentList.RemoveComponent(entityID);
}

void Sprite::RemoveAllComponents()
{
	m_componentList.RemoveAllComponents();
	m_vertexBuffer.~GraphicsBuffer();
	m_indexBuffer.~GraphicsBuffer();

	m_bInitializedSpriteQuad = false;
}

Sprite* Sprite::GetComponent(std::string entityID)
{
	return m_componentList.GetComponent(entityID);
}

void Sprite::Update(bool useThreads, int maxComponentsPerThread) {}

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

void Sprite::PreRender()
{
	if (!m_bInitializedSpriteQuad)
	{
		BufferHelper::CreateVertexBuffer(m_vertexBuffer, vertices);
		BufferHelper::CreateIndexBuffer(m_indexBuffer, indices);

		m_bInitializedSpriteQuad = true;
	}
}

void Sprite::Render(bool useThreads, int maxComponentsPerThread)
{
	m_componentList.Render(useThreads, maxComponentsPerThread);
}

void Sprite::OnRender()
{
	m_spriteTransform = Application::GetEntity(m_sEntityID)->GetComponent<Transform>();
	if (m_spriteTexture == nullptr)
		return;

	if (!m_bCreatedDescriptorSets)
	{
		BufferHelper::CreateUniformBuffers(m_uniformBuffers, m_uniformBuffersMapped);
		Renderer::GetGraphicsPipeline()->CreateDescriptorSets(m_descriptorSets, m_uniformBuffers, m_spriteTexture);

		m_bCreatedDescriptorSets = true;
	}

	UniformBufferObject ubo{};
	ubo.model = m_spriteTransform->m_transformMat; //This is worth moving to a push descriptor I think.
	ubo.view = Renderer::GenerateViewMatrix(); //Probably worth having UBO for Projection and View, bound once per frame.
	ubo.proj = Renderer::GenerateProjMatrix();

	memcpy(m_uniformBuffersMapped[Renderer::GetCurrentFrame()], &ubo, sizeof(ubo)); //Not the most efficient way to do this, refer back to conclusion of https://vulkan-tutorial.com/Uniform_buffers/Descriptor_layout_and_buffer

	//Bind vertex memory buffer to our command buffer, then draw it.
	VkBuffer vertexBuffers[] = { m_vertexBuffer.m_buffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(Renderer::GetCurrentCommandBuffer(), 0, 1, vertexBuffers, offsets);

	vkCmdBindIndexBuffer(Renderer::GetCurrentCommandBuffer(), m_indexBuffer.m_buffer, 0, VK_INDEX_TYPE_UINT32);

	vkCmdBindDescriptorSets(Renderer::GetCurrentCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, Renderer::GetGraphicsPipeline()->GetPipelineLayout(), 0, 1, &m_descriptorSets[Renderer::GetCurrentFrame()], 0, nullptr);
	vkCmdDrawIndexed(Renderer::GetCurrentCommandBuffer(), static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
}

void Sprite::LoadComponentDataFromJson(nlohmann::json& j)
{
	m_componentList.LoadComponentDataFromJson(j);
}

nlohmann::json Sprite::WriteComponentDataToJson()
{
	return m_componentList.WriteComponentDataToJson();
}