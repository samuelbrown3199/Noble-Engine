#include "Transform.h"

#include "../Core/Application.h"
#include "../ECS/Entity.hpp"

#include <glm/ext.hpp>

ComponentDatalist<Transform> Transform::m_componentList;

void Transform::AddComponent()
{
	m_componentList.AddComponent(this);
}

void Transform::AddComponentToEntity(std::string entityID)
{
	m_componentList.AddComponentToEntity(entityID);
	Application::GetEntity(entityID)->GetAllComponents();
}

void Transform::RemoveComponent(std::string entityID)
{
	m_componentList.RemoveComponent(entityID);
}

void Transform::RemoveAllComponents()
{
	m_componentList.RemoveAllComponents();
}

Transform* Transform::GetComponent(std::string entityID)
{
	return m_componentList.GetComponent(entityID);
}

void Transform::Update(bool useThreads, int maxComponentsPerThread)
{
	m_componentList.Update(useThreads, maxComponentsPerThread);
}

void Transform::OnUpdate()
{
	if (m_position != m_prevPosition || m_rotation != m_prevRotation || m_scale != m_prevScale)
	{
		m_transformMat = glm::mat4(1.0f);
		m_transformMat = glm::translate( m_transformMat,  m_position);
		m_transformMat = glm::rotate( m_transformMat, glm::radians( m_rotation.x), glm::vec3(1.0, 0.0, 0.0));
		m_transformMat = glm::rotate( m_transformMat, glm::radians( m_rotation.y), glm::vec3(0.0, 1.0, 0.0));
		m_transformMat = glm::rotate( m_transformMat, glm::radians( m_rotation.z), glm::vec3(0.0, 0.0, 1.0));
		m_transformMat = glm::scale( m_transformMat,  m_scale);

		m_prevPosition =  m_position;
		m_prevRotation =  m_rotation;
		m_prevScale =  m_scale;
	}
}

void Transform::Render(bool useThreads, int maxComponentsPerThread){}

void Transform::LoadComponentDataFromJson(nlohmann::json& j)
{
	m_componentList.LoadComponentDataFromJson(j);
}

nlohmann::json Transform::WriteComponentDataToJson()
{
	return m_componentList.WriteComponentDataToJson();
}