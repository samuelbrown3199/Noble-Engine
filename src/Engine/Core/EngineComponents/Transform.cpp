#include "Transform.h"

#include "../Application.h"
#include "../ECS/Entity.h"

#include <glm/ext.hpp>

void Transform::OnUpdate()
{
	if (m_parentTransformIndex == -1 && !m_bDoneParentCheck)
	{
		int entityIndex = Application::GetApplication()->GetEntityIndex(m_sEntityID);
		Entity* ent = Application::GetApplication()->GetEntity(entityIndex);

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
			NobleRegistry* registry = Application::GetApplication()->GetRegistry();

			Transform* parentTransform = registry->GetComponent<Transform>(m_parentTransformIndex);

			glm::mat4 parentMat = glm::mat4(1.0f);
			parentMat = glm::rotate(parentMat, glm::radians(-parentTransform->m_rotation.x), glm::vec3(1.0, 0.0, 0.0));
			parentMat = glm::rotate(parentMat, glm::radians(-parentTransform->m_rotation.y), glm::vec3(0.0, 1.0, 0.0));
			parentMat = glm::rotate(parentMat, glm::radians(-parentTransform->m_rotation.z), glm::vec3(0.0, 0.0, 1.0));
			parentMat = glm::scale(parentMat, parentTransform->m_scale);

			m_position = glm::vec4(m_localPosition + parentTransform->m_position, 1.0f) * parentMat;
			m_rotation = m_localRotation + parentTransform->m_rotation;
			m_scale = m_localScale * parentTransform->m_scale;
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