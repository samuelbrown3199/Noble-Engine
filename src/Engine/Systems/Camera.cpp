#include "Camera.h"

#include "../Core/Application.h"
#include "../ECS/Entity.hpp"
#include "Transform.h"

std::vector<Camera> Camera::m_componentData;

void Camera::AddComponent()
{
	m_componentData.push_back(*this);
}

void Camera::RemoveComponent(std::string entityID)
{
	for (int i = 0; i < m_componentData.size(); i++)
	{
		if (m_componentData.at(i).m_sEntityID == entityID)
			m_componentData.erase(m_componentData.begin() + i);
	}
}

void Camera::RemoveAllComponents()
{
	m_componentData.clear();
}

Camera* Camera::GetComponent(std::string entityID)
{
	for (int i = 0; i < m_componentData.size(); i++)
	{
		if (m_componentData.at(i).m_sEntityID == entityID)
			return &m_componentData.at(i);
	}

	return nullptr;
}

void Camera::PreUpdate()
{
	int bestCam = -1;
	CameraState bestState = inactive;

	for (int i = 0; i < m_componentData.size(); i++)
	{
		if (bestState <= m_componentData.at(i).m_state)
		{
			bestState = m_componentData.at(i).m_state;
			bestCam = i;
		}

		if (bestState == editorCam)
			break;
	}

	if (bestCam != -1)
		Renderer::SetCamera(&m_componentData.at(bestCam));
}

void Camera::Update(bool useThreads, int maxComponentsPerThread)
{
	if (!useThreads)
	{
		for (int i = 0; i < m_componentData.size(); i++)
		{
			m_componentData.at(i).OnUpdate();
		}
	}
	else
	{
		double amountOfThreads = ceil(m_componentData.size() / maxComponentsPerThread) + 1;
		for (int i = 0; i < amountOfThreads; i++)
		{
			int buffer = maxComponentsPerThread * i;
			auto th = ThreadingManager::EnqueueTask([&] { ThreadUpdate(buffer, maxComponentsPerThread); });
		}
	}
}

void Camera::ThreadUpdate(int _buffer, int _amount)
{
	int maxCap = _buffer + _amount;
	for (size_t co = _buffer; co < maxCap; co++)
	{
		if (co >= m_componentData.size())
			break;

		m_componentData.at(co).OnUpdate();
	}
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
void Camera::ThreadRender(int _buffer, int _amount) {}

void Camera::LoadComponentDataFromJson(nlohmann::json& j)
{
	for (auto it : j.items())
	{
		Camera component;
		component.m_sEntityID = it.key();
		component.FromJson(j[it.key()]);

		m_componentData.push_back(component);
	}
}

nlohmann::json Camera::WriteComponentDataToJson()
{
	nlohmann::json data;

	for (int i = 0; i < m_componentData.size(); i++)
	{
		data[m_componentData.at(i).m_sEntityID] = m_componentData.at(i).WriteJson();
	}

	return data;
}