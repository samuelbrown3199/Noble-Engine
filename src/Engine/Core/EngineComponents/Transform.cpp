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
		float position[3] = { m_localPosition.x, m_localPosition.y, m_localPosition.z };
		if (ImGui::InputFloat3("Local Position", position, "%.3f"))
		{
			ChangeValueCommand<glm::vec3>* command = new ChangeValueCommand<glm::vec3>(&m_localPosition, glm::vec3(position[0], position[1], position[2]));
			Application::GetApplication()->PushCommand(command);
		}

		float rotation[3] = { m_localRotation.x, m_localRotation.y, m_localRotation.z };
		if (ImGui::InputFloat3("Local Rotation", rotation, "%.3f"))
		{
			ChangeValueCommand<glm::vec3>* command = new ChangeValueCommand<glm::vec3>(&m_localRotation, glm::vec3(rotation[0], rotation[1], rotation[2]));
			Application::GetApplication()->PushCommand(command);
		}

		float scale[3] = { m_localScale.x, m_localScale.y, m_localScale.z };
		if (ImGui::InputFloat3("Local Scale", scale, "%.3f"))
		{
			ChangeValueCommand<glm::vec3>* command = new ChangeValueCommand<glm::vec3>(&m_localScale, glm::vec3(scale[0], scale[1], scale[2]));
			Application::GetApplication()->PushCommand(command);
		}
		
		return;
	}

	//Handle normal Transform data
	float position[3] = {m_position.x, m_position.y, m_position.z};
	if (ImGui::InputFloat3("Position", position, "%.3f"))
	{
		ChangeValueCommand<glm::vec3>* command = new ChangeValueCommand<glm::vec3>(&m_position, glm::vec3(position[0], position[1], position[2]));
		Application::GetApplication()->PushCommand(command);
	}

	float rotation[3] = { m_rotation.x, m_rotation.y, m_rotation.z };
	if (ImGui::InputFloat3("Rotation", rotation, "%.3f"))
	{
		ChangeValueCommand<glm::vec3>* command = new ChangeValueCommand<glm::vec3>(&m_rotation, glm::vec3(rotation[0], rotation[1], rotation[2]));
		Application::GetApplication()->PushCommand(command);
	}

	float scale[3] = { m_scale.x, m_scale.y, m_scale.z };
	if (ImGui::InputFloat3("Scale", scale, "%.3f"))
	{
		ChangeValueCommand<glm::vec3>* command = new ChangeValueCommand<glm::vec3>(&m_scale, glm::vec3(scale[0], scale[1], scale[2]));
		Application::GetApplication()->PushCommand(command);
	}
}