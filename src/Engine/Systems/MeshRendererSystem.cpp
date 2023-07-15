#include "MeshRendererSystem.h"
#include "../Core/Application.h"
#include "../Core/Graphics/Renderer.h"
#include "../Core/Graphics/GraphicsPipeline.h"

#include <iostream>

std::weak_ptr<SystemBase> MeshRendererSystem::self;
std::weak_ptr<SystemBase> MeshRenderer::componentSystem;
std::vector<MeshRenderer> MeshRenderer::componentData;

void MeshRendererSystem::CreateDescriptor(MeshRenderer* comp)
{
	/*std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, Renderer::GetGraphicsPipeline()->GetDescriptorSetLayout());
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = m_descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	allocInfo.pSetLayouts = layouts.data();

	m_descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
	if (vkAllocateDescriptorSets(Renderer::GetLogicalDevice(), &allocInfo, m_descriptorSets.data()) != VK_SUCCESS)
	{
		Logger::LogError("Failed to allocate descriptor sets.", 2);
	}

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = m_uniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = comp->m_texture->m_textureImageView;
		imageInfo.sampler = comp->m_texture->m_textureSampler;

		std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = m_descriptorSets[i];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = m_descriptorSets[i];
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(Renderer::GetLogicalDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}*/
}

void MeshRendererSystem::OnRender(MeshRenderer* comp)
{
	if (comp->m_transform == nullptr || Application::GetEntitiesDeleted())
	{
		comp->m_transform = Transform::GetComponent(comp->m_sEntityID);
	}

	if (comp->m_model == nullptr)
		return;

	UniformBufferObject ubo{};
	ubo.model = comp->m_transform->m_transformMat;
	ubo.view = Renderer::GenerateViewMatrix();
	ubo.proj = Renderer::GenerateProjMatrix();

	memcpy(Renderer::GetCurrentUniformBuffer(), &ubo, sizeof(ubo)); //Not the most efficient way to do this, refer back to conclusion of https://vulkan-tutorial.com/Uniform_buffers/Descriptor_layout_and_buffer

	//Bind vertex memory buffer to our command buffer, then draw it.
	VkBuffer vertexBuffers[] = { comp->m_model->m_vertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(Renderer::GetCurrentCommandBuffer(), 0, 1, vertexBuffers, offsets);

	vkCmdBindIndexBuffer(Renderer::GetCurrentCommandBuffer(), comp->m_model->m_indexBuffer, 0, VK_INDEX_TYPE_UINT32);

	VkDescriptorSet descSet = Renderer::GetCurrentDescriptorSet();
	vkCmdBindDescriptorSets(Renderer::GetCurrentCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, Renderer::GetGraphicsPipeline()->GetPipelineLayout(), 0, 1, &descSet, 0, nullptr);
	//Command to draw our triangle. Parameters as follows, CommandBuffer, IndexCount, InstanceCount, firstIndex, vertexOffset, firstInstance
	vkCmdDrawIndexed(Renderer::GetCurrentCommandBuffer(), static_cast<uint32_t>(comp->m_model->m_indices.size()), 1, 0, 0, 0);
}