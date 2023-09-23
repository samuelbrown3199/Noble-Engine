#pragma once

#include "Component.hpp"

#include "../Core/Application.h"
#include "../Core/Graphics/Renderer.h"
#include "../ECS/Entity.hpp"

#include "../Components/Transform.h"

struct Renderable : public Component
{
	Transform* m_transform = nullptr;
	std::vector<Vertex>* m_vertices;
	std::vector<glm::vec3>* m_boundingBox;

	bool m_bOnScreen = false;

	virtual void PreRender() {};
	virtual void Render(bool useThreads, int maxComponentsPerThread) = 0;
	virtual void OnRender() 
	{
		if (m_transform == nullptr)
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
	}
};