#pragma once

#include "Component.h"

#include "../Core/Application.h"
#include "../Core/Graphics/Renderer.h"
#include "../ECS/Entity.hpp"

#include "../Components/Camera.h"
#include "../Components/Transform.h"

struct Renderable : public Component
{
	Transform* m_transform = nullptr;
	std::vector<Vertex>* m_vertices;
	std::vector<uint32_t>* m_indices;
	std::vector<glm::vec3>* m_boundingBox;

	bool m_bOnScreen = false;
	float m_fDistanceToCam;

	virtual void PreRender() {};
	virtual void OnRender() 
	{
		if (m_transform == nullptr || Application::GetEntitiesDeleted())
		{
			m_transform = Application::GetEntity(m_sEntityID)->GetComponent<Transform>();
			m_bOnScreen = false;
			return;
		}

		m_bOnScreen = false;
		for (int i = 0; i < m_boundingBox->size(); i++)
		{
			glm::vec3 transPos = m_transform->m_transformMat * glm::vec4(m_boundingBox->at(i), 1.0f);
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

		Camera* cam = Renderer::GetCamera();
		if (cam != nullptr)
		{
			m_fDistanceToCam = glm::length(cam->m_camTransform->m_position - m_transform->m_position);
		}
	}
};