#include "Camera.h"

#include "../Core/Application.h"
#include "../ECS/Entity.hpp"
#include "Transform.h"

ComponentDatalist<Camera> Camera::m_componentList;

void Camera::AddComponent()
{
	m_componentList.AddComponent(this);
}

void Camera::RemoveComponent(std::string entityID)
{
	m_componentList.RemoveComponent(entityID);
}

void Camera::RemoveAllComponents()
{
	m_componentList.RemoveAllComponents();
}

Camera* Camera::GetComponent(std::string entityID)
{
	return m_componentList.GetComponent(entityID);
}

void Camera::Update(bool useThreads, int maxComponentsPerThread)
{
	m_componentList.Update(useThreads, maxComponentsPerThread);
}

void Camera::PreUpdate()
{
	int bestCam = -1;
	CameraState bestState = inactive;

	for (int i = 0; i < m_componentList.m_componentData.size(); i++)
	{
		if (bestState <= m_componentList.m_componentData.at(i).m_state)
		{
			bestState = m_componentList.m_componentData.at(i).m_state;
			bestCam = i;
		}

		if (bestState == editorCam)
			break;
	}

	if (bestCam != -1)
		Renderer::SetCamera(&m_componentList.m_componentData.at(bestCam));
}

void Camera::OnUpdate()
{
	if (m_camTransform == nullptr)
	{
		m_camTransform = Application::GetEntity(m_sEntityID)->GetComponent<Transform>();

		if (m_camTransform == nullptr)
			return;

		//Set an initial value if needed.
		if (m_camTransform->m_rotation == glm::vec3(0, 0, 0))
			m_camTransform->m_rotation = glm::vec3(0, 0, -1);
	}
	m_camTransform->m_rotation = glm::normalize(m_camTransform->m_rotation);
}

void Camera::Render(bool useThreads, int maxComponentsPerThread) {}

void Camera::LoadComponentDataFromJson(nlohmann::json& j)
{
	m_componentList.LoadComponentDataFromJson(j);
}

nlohmann::json Camera::WriteComponentDataToJson()
{
	return m_componentList.WriteComponentDataToJson();
}