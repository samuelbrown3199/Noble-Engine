#include "Transform.h"

#include "../Application.h"
#include "../ECS/Entity.h"
#include "../CommandTypes.h"

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

void Transform::DoComponentInterface()
{
	//Handle Child Transform data
	if(m_parentTransformIndex != -1)
	{
		static NobleDragFloat localPos;
		localPos.m_pComponent = this;
		localPos.DoDragFloat3("Local Position", m_bInitializeInterface, &m_localPosition, 0.1f);

		static NobleDragFloat localRot;
		localRot.m_pComponent = this;
		localRot.DoDragFloat3("Local Rotation", m_bInitializeInterface, &m_localRotation, 0.1f);

		static NobleDragFloat localScale;
		localScale.m_pComponent = this;
		localScale.DoDragFloat3("Local Scale", m_bInitializeInterface, &m_localScale, 0.1f);
		
		m_bInitializeInterface = false;

		return;
	}

	static NobleDragFloat pos;
	pos.m_pComponent = this;
	pos.DoDragFloat3("Position", m_bInitializeInterface, &m_position, 0.1f);

	static NobleDragFloat rot;
	rot.m_pComponent = this;
	rot.DoDragFloat3("Rotation", m_bInitializeInterface, &m_rotation, 0.1f);

	static NobleDragFloat scale;
	scale.m_pComponent = this;
	scale.DoDragFloat3("Scale", m_bInitializeInterface, &m_scale, 0.1f);

	m_bInitializeInterface = false;
}