#include "Sprite.h"
#include "../Core/Application.h"
#include "../Core/Graphics/Renderer.h"
#include "../Core/Graphics/BufferHelper.h"
#include "../Core/Graphics/GraphicsPipeline.h"
#include "../ECS/Entity.hpp"

bool Sprite::m_bInitializedSpriteQuad = false;
GraphicsBuffer Sprite::m_vertexBuffer;
GraphicsBuffer Sprite::m_indexBuffer;
std::vector<Sprite> Sprite::m_componentData;

void Sprite::AddComponent()
{
	m_componentData.push_back(*this);
}

void Sprite::RemoveComponent(std::string entityID)
{
	for (int i = 0; i < m_componentData.size(); i++)
	{
		if (m_componentData.at(i).m_sEntityID == entityID)
			m_componentData.erase(m_componentData.begin() + i);
	}
}

void Sprite::RemoveAllComponents()
{
	m_componentData.clear();

	m_vertexBuffer.~GraphicsBuffer();
	m_indexBuffer.~GraphicsBuffer();

	m_bInitializedSpriteQuad = false;
}

Sprite* Sprite::GetComponent(std::string entityID)
{
	for (int i = 0; i < m_componentData.size(); i++)
	{
		if (m_componentData.at(i).m_sEntityID == entityID)
			return &m_componentData.at(i);
	}

	return nullptr;
}

void Sprite::Update(bool useThreads, int maxComponentsPerThread)
{
	if (!useThreads)
	{
		for (int i = 0; i < m_componentData.size(); i++)
		{
			m_componentData.at(i).OnUpdate();
		}
	}
	else
	{
		double amountOfThreads = ceil(m_componentData.size() / maxComponentsPerThread) + 1;
		for (int i = 0; i < amountOfThreads; i++)
		{
			int buffer = maxComponentsPerThread * i;
			auto th = ThreadingManager::EnqueueTask([&] { ThreadUpdate(buffer, maxComponentsPerThread); });
		}
	}
}

void Sprite::ThreadUpdate(int _buffer, int _amount) {}

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
	if (!useThreads)
	{
		for (int i = 0; i < m_componentData.size(); i++)
		{
			m_componentData.at(i).OnRender();
		}
	}
	else
	{
		double amountOfThreads = ceil(m_componentData.size() / maxComponentsPerThread) + 1;
		for (int i = 0; i < amountOfThreads; i++)
		{
			int buffer = maxComponentsPerThread * i;
			auto th = ThreadingManager::EnqueueTask([&] { ThreadRender(buffer, maxComponentsPerThread); });
		}
	}
}

void Sprite::ThreadRender(int _buffer, int _amount)
{
	int maxCap = _buffer + _amount;
	for (size_t co = _buffer; co < maxCap; co++)
	{
		if (co >= m_componentData.size())
			break;

		m_componentData.at(co).OnRender();
	}
}

void Sprite::OnRender()
{
	if (m_spriteTransform == nullptr || Application::GetEntitiesDeleted())
	{
		m_spriteTransform = Application::GetEntity(m_sEntityID)->GetComponent<Transform>();
		return;
	}
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
	for (auto it : j.items())
	{
		Sprite  component;
		component.m_sEntityID = it.key();
		component.FromJson(j[it.key()]);

		m_componentData.push_back(component);
	}
}

nlohmann::json Sprite::WriteComponentDataToJson()
{
	nlohmann::json data;

	for (int i = 0; i < m_componentData.size(); i++)
	{
		data[m_componentData.at(i).m_sEntityID] = m_componentData.at(i).WriteJson();
	}

	return data;
}