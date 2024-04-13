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

	glm::vec4 m_colour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	bool m_bOnScreen = false;
	float m_fDistanceToCam;

	virtual void OnRemove() override
	{
		m_transformIndex = -1;
	}

	void ChangeTexture(std::shared_ptr<Texture> sprite)
	{
		if (sprite == nullptr)
			return;

		if (m_texture != nullptr && sprite->m_sLocalPath == m_texture->m_sLocalPath)
			return;

		m_texture = sprite;
	}

	void ChangePipeline(std::shared_ptr<Pipeline> pipeline)
	{
		if (pipeline == nullptr)
			return;

		if (m_pipeline != nullptr && m_pipeline->m_sLocalPath == pipeline->m_sLocalPath)
			return;

		m_pipeline = pipeline;
	}


	virtual void OnPreRender() 
	{
		Renderer::IncrementRenderables();

		NobleRegistry* registry = Application::GetApplication()->GetRegistry();

		if (m_transformIndex == -1)
		{
			m_transformIndex = registry->GetComponentIndex<Transform>(m_sEntityID);
			m_bOnScreen = false;
			return;
		}

		Transform* transform = registry->GetComponent<Transform>(m_transformIndex);
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
		m_drawConstants.m_objectColour = m_colour;
		m_drawConstants.m_worldMatrix =  transform->m_transformMat;

		Camera* cam = Renderer::GetCamera();
		if (cam != nullptr)
		{
			Transform* camTransform = registry->GetComponent<Transform>(cam->m_camTransformIndex);
			m_fDistanceToCam = glm::length(camTransform->m_position - transform->m_position);
		}
	};

	virtual void OnRender(VkCommandBuffer cmd)
	{
		if (m_pipeline == nullptr || !m_pipeline->m_bIsLoaded)
			return;

		Renderer* renderer = Application::GetApplication()->GetRenderer();

		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->m_pipeline);

		NobleRegistry* registry = Application::GetApplication()->GetRegistry();
		std::vector<VkDescriptorSet> sets;
		for (int i = 0; i < m_pipeline->m_vDescriptors.size(); i++)
		{
			VkDescriptorSet targetSet;

			if (m_pipeline->m_vDescriptors.at(i).second->m_set != nullptr)
			{
				targetSet = *m_pipeline->m_vDescriptors.at(i).second->m_set;
				sets.push_back(targetSet);
			}
			else
			{
				targetSet = renderer->GetCurrentFrame().m_frameDescriptors.AllocateSet(renderer->GetLogicalDevice(), *m_pipeline->m_vDescriptors.at(i).second->m_layout);
				sets.push_back(targetSet);
			}

			if (m_pipeline->m_vDescriptors.at(i).second->m_type == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) //HARD CODED FOR NOW, MATERIAL SYSTEM WILL CHANGE THIS.
			{
				DescriptorWriter writer;
				if (m_texture != nullptr)
				{
					writer.WriteImage(0, m_texture->m_texture.m_imageView, m_texture->m_textureSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
					writer.UpdateSet(renderer->GetLogicalDevice(), targetSet);
				}
				else
				{
					writer.WriteImage(0, renderer->GetCheckerboardErrorTexture().m_imageView, renderer->GetDefaultSampler(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
					writer.UpdateSet(renderer->GetLogicalDevice(), targetSet);
				}
			}
		}

		for (int i = 0; i < m_pipeline->m_vPushConstants.size(); i++) //HARD CODED FOR NOW, MATERIAL SYSTEM WILL CHANGE THIS.
		{
			vkCmdPushConstants(cmd, m_pipeline->m_pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(GPUDrawPushConstants), &m_drawConstants);
		}

		vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->m_pipelineLayout, 0, sets.size(), sets.data(), 0, nullptr);
		vkCmdBindIndexBuffer(cmd, m_meshBuffers.m_indexBuffer.m_buffer, 0, VK_INDEX_TYPE_UINT32);

		vkCmdDrawIndexed(cmd, m_indices->size(), 1, 0, 0, 0);
	}
};