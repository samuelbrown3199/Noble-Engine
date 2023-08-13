#include "Transform.h"

#include "../Core/Application.h"

#include <glm/ext.hpp>

std::vector<Transform> Transform::m_componentData;

void Transform::AddComponent()
{
	m_componentData.push_back(*this);
}

void Transform::RemoveComponent(std::string entityID)
{
	for (int i = 0; i < m_componentData.size(); i++)
	{
		if (m_componentData.at(i).m_sEntityID == entityID)
			m_componentData.erase(m_componentData.begin() + i);
	}
}

void Transform::RemoveAllComponents()
{
	m_componentData.clear();
}

Transform* Transform::GetComponent(std::string entityID)
{
	for (int i = 0; i < m_componentData.size(); i++)
	{
		if (m_componentData.at(i).m_sEntityID == entityID)
			return &m_componentData.at(i);
	}

	return nullptr;
}

void Transform::Update(bool useThreads, int maxComponentsPerThread)
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

void Transform::ThreadUpdate(int _buffer, int _amount)
{
	int maxCap = _buffer + _amount;
	for (size_t co = _buffer; co < maxCap; co++)
	{
		if (co >= m_componentData.size())
			break;

		m_componentData.at(co).OnUpdate();
	}
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
void Transform::ThreadRender(int _buffer, int _amount) {}

void Transform::LoadComponentDataFromJson(nlohmann::json& j)
{
	for (auto it : j.items())
	{
		Transform component;
		component.m_sEntityID = it.key();
		component.FromJson(j[it.key()]);

		m_componentData.push_back(component);
	}
}

nlohmann::json Transform::WriteComponentDataToJson()
{
	nlohmann::json data;

	for (int i = 0; i < m_componentData.size(); i++)
	{
		data[m_componentData.at(i).m_sEntityID] = m_componentData.at(i).WriteJson();
	}

	return data;
}