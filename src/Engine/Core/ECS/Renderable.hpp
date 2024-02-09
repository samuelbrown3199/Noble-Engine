#pragma once

#include "Component.h"

#include "../Application.h"
#include "../Graphics/Renderer.h"
#include "../ECS/Entity.hpp"

#include "../EngineComponents/Camera.h"
#include "../EngineComponents/Transform.h"

struct Renderable : public Component
{
	int m_transformIndex = -1;
	std::vector<Vertex>* m_vertices;
	std::vector<uint32_t>* m_indices;
	std::vector<glm::vec3>* m_boundingBox;

	GPUMeshBuffers m_meshBuffers;
	GPUDrawPushConstants m_drawConstants;

	bool m_bOnScreen = false;
	float m_fDistanceToCam;

	virtual void OnRemove() override
	{
		m_transformIndex = -1;
	}

	virtual void PreRender() {};
	virtual void OnRender() 
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
	}
};