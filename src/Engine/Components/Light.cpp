#include "Light.h"

#include "../Core/Application.h"
#include "../ECS/Entity.hpp"

ComponentDatalist<Light> Light::m_componentList;
std::vector<Light*> Light::m_closestLights;
int Light::m_iMaxLights = 20;

void Light::AddComponent()
{
	m_componentList.AddComponent(this);
}

void Light::AddComponentToEntity(std::string entityID)
{
	m_componentList.AddComponentToEntity(entityID);
	Application::GetEntity(entityID)->GetAllComponents();
}

void Light::RemoveComponent(std::string entityID)
{
	m_componentList.RemoveComponent(entityID);
}

void Light::RemoveAllComponents()
{
	m_componentList.RemoveAllComponents();
}

Light* Light::GetComponent(std::string entityID)
{
	return m_componentList.GetComponent(entityID);
}

bool SortByDistance(Light* val1, Light* val2)
{
	return val1->m_fDistanceToCam < val2->m_fDistanceToCam;
}

void Light::PreUpdate() //probably a far better way to do this. Legacy code begging for update.
{
	m_closestLights.clear();

	for (int i = 0; i < m_componentList.m_componentData.size(); i++)
	{
		if (m_componentList.m_componentData.at(i).m_bAvailableForReuse)
			continue;

		if (i < m_iMaxLights)
		{
			m_closestLights.push_back(&m_componentList.m_componentData.at(i));
			std::sort(m_closestLights.begin(), m_closestLights.end(), SortByDistance);
		}
		else
		{
			Light* curLight = &m_componentList.m_componentData.at(i);
			if (curLight->m_fDistanceToCam < m_closestLights.back()->m_fDistanceToCam)
			{
				m_closestLights.pop_back();
				m_closestLights.push_back(&m_componentList.m_componentData.at(i));
				std::sort(m_closestLights.begin(), m_closestLights.end(), SortByDistance);
				break;
			}
		}
	}

	std::vector<std::shared_ptr<ShaderProgram>>* shaderPrograms = ResourceManager::GetShaderPrograms();
	for (int i = 0; i < shaderPrograms->size(); i++)
	{
		shaderPrograms->at(i)->BindInt("numberOfLights", m_closestLights.size());
	}

	for (int i = 0; i < m_closestLights.size(); i++)
	{
		if (m_closestLights.at(i)->m_transform == nullptr)
			continue;

		m_closestLights.at(i)->m_lightInfo->BindInfoToShaders(i, m_closestLights.at(i)->m_transform);
	}
}

void Light::Update(bool useThreads, int maxComponentsPerThread)
{
	m_componentList.Update(useThreads, maxComponentsPerThread);
}

void Light::OnUpdate()
{
	m_transform = Application::GetEntity(m_sEntityID)->GetComponent<Transform>();
	if (m_transform == nullptr)
		return;

	switch (m_lightType)
	{
	case Directional:
		DirectionalLight* dirLight = dynamic_cast<DirectionalLight*>(m_lightInfo);
		dirLight->m_direction = glm::normalize(m_transform->m_rotation);
		break;
	}

	float xDis = (Renderer::GetCamera()->m_camTransform->m_position.x - m_transform->m_position.x) * (Renderer::GetCamera()->m_camTransform->m_position.x - m_transform->m_position.x);
	float yDis = (Renderer::GetCamera()->m_camTransform->m_position.y - m_transform->m_position.y) * (Renderer::GetCamera()->m_camTransform->m_position.y - m_transform->m_position.y);
	float zDis = (Renderer::GetCamera()->m_camTransform->m_position.z - m_transform->m_position.z) * (Renderer::GetCamera()->m_camTransform->m_position.z - m_transform->m_position.z);

	m_fDistanceToCam = sqrt(xDis + yDis + zDis);
}

void Light::Render(bool useThreads, int maxComponentsPerThread) {}

void Light::LoadComponentDataFromJson(nlohmann::json& j)
{
	m_componentList.LoadComponentDataFromJson(j);
}

nlohmann::json Light::WriteComponentDataToJson()
{
	return m_componentList.WriteComponentDataToJson();
}