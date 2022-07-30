#include "TransformSystem.h"

#include <glm/ext.hpp>

std::weak_ptr<SystemBase> TransformSystem::self;
std::weak_ptr<SystemBase> Transform::componentSystem;
std::vector<Transform> Transform::componentData;

void TransformSystem::OnUpdate(Transform* comp)
{
	if (comp->m_position != comp->m_prevPosition || comp->m_rotation != comp->m_prevRotation || comp->m_scale != comp->m_prevScale)
	{
		comp->m_transformMat = glm::mat4(1.0f);
		comp->m_transformMat = glm::translate(comp->m_transformMat, comp->m_position);
		comp->m_transformMat = glm::rotate(comp->m_transformMat, glm::radians(comp->m_rotation.x), glm::vec3(1.0, 0.0, 0.0));
		comp->m_transformMat = glm::rotate(comp->m_transformMat, glm::radians(comp->m_rotation.y), glm::vec3(0.0, 1.0, 0.0));
		comp->m_transformMat = glm::rotate(comp->m_transformMat, glm::radians(comp->m_rotation.z), glm::vec3(0.0, 0.0, 1.0));
		comp->m_transformMat = glm::scale(comp->m_transformMat, comp->m_scale);

		comp->m_prevPosition = comp->m_position;
		comp->m_prevRotation = comp->m_rotation;
		comp->m_prevScale = comp->m_scale;
	}
}