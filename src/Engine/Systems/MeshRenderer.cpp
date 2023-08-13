#include "MeshRenderer.h"

#include "../Core/Application.h"
#include "../Core/Graphics/Renderer.h"
#include "../Core/Graphics/BufferHelper.h"
#include "../Core/Graphics/GraphicsPipeline.h"
#include "../ECS/Entity.hpp"

std::vector<MeshRenderer> MeshRenderer::m_componentData;

void MeshRenderer::AddComponent()
{
	m_componentData.push_back(*this);
}

void MeshRenderer::RemoveComponent(std::string entityID)
{
	for (int i = 0; i < m_componentData.size(); i++)
	{
		if (m_componentData.at(i).m_sEntityID == entityID)
			m_componentData.erase(m_componentData.begin() + i);
	}
}

void MeshRenderer ::RemoveAllComponents()
{
	m_componentData.clear();
}

MeshRenderer* MeshRenderer::GetComponent(std::string entityID)
{
	for (int i = 0; i < m_componentData.size(); i++)
	{
		if (m_componentData.at(i).m_sEntityID == entityID)
			return &m_componentData.at(i);
	}

	return nullptr;
}

void MeshRenderer::Update(bool useThreads, int maxComponentsPerThread) {}
void MeshRenderer::ThreadUpdate(int _buffer, int _amount){}

void MeshRenderer::Render(bool useThreads, int maxComponentsPerThread)
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

void MeshRenderer::ThreadRender(int _buffer, int _amount)
{
	int maxCap = _buffer + _amount;
	for (size_t co = _buffer; co < maxCap; co++)
	{
		if (co >= m_componentData.size())
			break;

		m_componentData.at(co).OnRender();
	}
}

void MeshRenderer::OnRender() 
{
	if (m_transform == nullptr || Application::GetEntitiesDeleted())
	{
		m_transform = Application::GetEntity(m_sEntityID)->GetComponent<Transform>();
	}

	if (m_model == nullptr)
		return;

	if (!m_bCreatedDescriptorSets)
	{
		BufferHelper::CreateUniformBuffers(m_uniformBuffers, m_uniformBuffersMapped);
		Renderer::GetGraphicsPipeline()->CreateDescriptorSets(m_descriptorSets, m_uniformBuffers, m_texture);

		m_bCreatedDescriptorSets = true;
	}

	UniformBufferObject ubo{};
	ubo.model = m_transform->m_transformMat; //This is worth moving to a push descriptor I think.
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

void MeshRenderer::LoadComponentDataFromJson(nlohmann::json& j)
{
	for (auto it : j.items())
	{
		MeshRenderer  component;
		component.m_sEntityID = it.key();
		component.FromJson(j[it.key()]);

		m_componentData.push_back(component);
	}
}

nlohmann::json MeshRenderer::WriteComponentDataToJson()
{
	nlohmann::json data;

	for (int i = 0; i < m_componentData.size(); i++)
	{
		data[m_componentData.at(i).m_sEntityID] = m_componentData.at(i).WriteJson();
	}

	return data;
}