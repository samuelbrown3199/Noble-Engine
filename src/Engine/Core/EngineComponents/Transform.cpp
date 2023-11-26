#include "Transform.h"

#include "../Application.h"
#include "../ECS/Entity.hpp"

#include <glm/ext.hpp>

void Transform::OnUpdate()
{
	if (m_parentTransformIndex == -1 && !m_bDoneParentCheck)
	{
		int entityIndex = Application::GetEntityIndex(m_sEntityID);
		Entity* ent = Application::GetEntity(entityIndex);

		if (ent->m_sEntityParentID != "")
		{
			m_parentTransformIndex = Transform::GetComponentIndex(ent->m_sEntityParentID);
		}

		m_bDoneParentCheck = true;
	}

	if (m_position != m_prevPosition || m_rotation != m_prevRotation || m_scale != m_prevScale || m_parentTransformIndex != -1)
	{
		if (m_parentTransformIndex != -1)
		{
			Transform* parentTransform = NobleRegistry::GetComponent<Transform>(m_parentTransformIndex);

			m_position = m_localPosition + parentTransform->m_position;
			m_rotation = m_localRotation + parentTransform->m_rotation;
			m_scale = m_localScale + parentTransform->m_scale;
		}

		m_transformMat = glm::mat4(1.0f);
		m_transformMat = glm::translate(m_transformMat, m_position);
		m_transformMat = glm::rotate(m_transformMat, glm::radians(m_rotation.x), glm::vec3(1.0, 0.0, 0.0));
		m_transformMat = glm::rotate(m_transformMat, glm::radians(m_rotation.y), glm::vec3(0.0, 1.0, 0.0));
		m_transformMat = glm::rotate(m_transformMat, glm::radians(m_rotation.z), glm::vec3(0.0, 0.0, 1.0));
		m_transformMat = glm::scale(m_transformMat, m_scale);

		m_prevPosition =  m_position;
		m_prevRotation =  m_rotation;
		m_prevScale =  m_scale;
	}
}