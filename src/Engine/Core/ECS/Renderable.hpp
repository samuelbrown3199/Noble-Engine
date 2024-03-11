#pragma once

#include "Component.h"

#include "../Application.h"
#include "../Graphics/Renderer.h"
#include "../ECS/Entity.hpp"

#include "../EngineComponents/Camera.h"
#include "../EngineComponents/Transform.h"

#include "../EngineResources/Pipeline.h"

struct Renderable : public Component
{
	int m_transformIndex = -1;
	std::vector<Vertex>* m_vertices;
	std::vector<uint32_t>* m_indices;
	std::vector<glm::vec3>* m_boundingBox;

	std::shared_ptr<Texture> m_texture = nullptr;

	std::shared_ptr<Pipeline> m_pipeline;

	GPUMeshBuffers m_meshBuffers;
	GPUDrawPushConstants m_drawConstants;

	bool m_bOnScreen = false;
	float m_fDistanceToCam;

	virtual void OnRemove() override
	{
		m_transformIndex = -1;
	}

	virtual void OnPreRender() 
	{
		Renderer::IncrementRenderables();

		if (m_transformIndex == -1)
		{
			m_transformIndex = NobleRegistry::GetComponentIndex<Transform>(m_sEntityID);
			m_bOnScreen = false;
			return;
		}

		Transform* transform = NobleRegistry::GetComponent<Transform>(m_transformIndex);
		if (transform == nullptr)
			return;

		m_bOnScreen = false;
		for (int i = 0; i < m_boundingBox->size(); i++)
		{
			glm::vec3 transPos = transform->m_transformMat * glm::vec4(m_boundingBox->at(i), 1.0f);
			if (IsPointInViewFrustum(transPos, Renderer::GenerateProjMatrix() * Renderer::GenerateViewMatrix()))
			{
				m_bOnScreen = true;
				break;
			}
		}

		if (m_bOnScreen)
			Renderer::AddOnScreenObject(this);
		else
			return;

		m_drawConstants.m_vertexBuffer = m_meshBuffers.m_vertexBufferAddress;

		Camera* cam = Renderer::GetCamera();
		if (cam != nullptr)
		{
			Transform* camTransform = NobleRegistry::GetComponent<Transform>(cam->m_camTransformIndex);
			m_fDistanceToCam = glm::length(camTransform->m_position - transform->m_position);
		}
	};

	virtual void OnRender(VkCommandBuffer cmd)
	{
		Renderer* renderer = Application::GetRenderer();

		VkDescriptorSet imageSet = renderer->GetCurrentFrame().m_frameDescriptors.AllocateSet(renderer->GetLogicalDevice(), renderer->m_singleImageDescriptorLayout);

		DescriptorWriter writer;
		if (m_texture != nullptr)
		{
			writer.WriteImage(0, m_texture->m_texture.m_imageView, m_texture->m_textureSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
			writer.UpdateSet(renderer->GetLogicalDevice(), imageSet);
		}
		else
		{
			writer.WriteImage(0, renderer->GetCheckerboardErrorTexture().m_imageView, renderer->GetDefaultSampler(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
			writer.UpdateSet(renderer->GetLogicalDevice(), imageSet);
		}

		if (m_pipeline == nullptr)
			return;

		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->m_pipeline);

		vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, renderer->m_meshPipeline->m_pipelineLayout, 0, 1, &imageSet, 0, nullptr);

		vkCmdPushConstants(cmd, renderer->m_meshPipeline->m_pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(GPUDrawPushConstants), &m_drawConstants);
		vkCmdBindIndexBuffer(cmd, m_meshBuffers.m_indexBuffer.m_buffer, 0, VK_INDEX_TYPE_UINT32);

		vkCmdDrawIndexed(cmd, m_indices->size(), 1, 0, 0, 0);
	}
};