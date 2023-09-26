#include "Light.h"

#include "../Core/Application.h"
#include "../ECS/Entity.hpp"

#include "../ECS/ComponentList.hpp"

std::vector<Light*> Light::m_closestLights;
int Light::m_iMaxLights = 20;

bool SortByDistance(Light* val1, Light* val2)
{
	return val1->m_fDistanceToCam < val2->m_fDistanceToCam;
}

void Light::PreUpdate() //probably a far better way to do this. Legacy code begging for update.
{
	m_closestLights.clear();

	int numPointLights = 0;
	int numSpotLights = 0;

	ComponentDatalist<Light>* dataList = dynamic_cast<ComponentDatalist<Light>*>(NobleRegistry::GetComponentList(GetComponentID()));

	for (int i = 0; i < dataList->m_componentData.size(); i++)
	{
		if (dataList->m_componentData.at(i).m_bAvailableForReuse)
			continue;

		if (m_closestLights.size() < m_iMaxLights)
		{
			m_closestLights.push_back(&dataList->m_componentData.at(i));
			std::sort(m_closestLights.begin(), m_closestLights.end(), SortByDistance);
		}
		else
		{
			Light* curLight = &dataList->m_componentData.at(i);
			if (curLight->m_fDistanceToCam < m_closestLights.back()->m_fDistanceToCam)
			{
				m_closestLights.pop_back();
				m_closestLights.push_back(&dataList->m_componentData.at(i));
				std::sort(m_closestLights.begin(), m_closestLights.end(), SortByDistance);
				break;
			}
		}
	}

	for (int i = 0; i < m_closestLights.size(); i++)
	{
		if (m_closestLights.at(i)->m_transform == nullptr)
			continue;

		if (m_closestLights.at(i)->m_lightType == Point)
		{
			m_closestLights.at(i)->m_lightInfo->BindInfoToShaders(numPointLights, m_closestLights.at(i)->m_transform);
			numPointLights++;
		}
		else if (m_closestLights.at(i)->m_lightType == Spot)
		{
			m_closestLights.at(i)->m_lightInfo->BindInfoToShaders(numSpotLights, m_closestLights.at(i)->m_transform);
			numSpotLights++;
		}
	}

	std::vector<std::shared_ptr<ShaderProgram>>* shaderPrograms = ResourceManager::GetShaderPrograms();
	for (int i = 0; i < shaderPrograms->size(); i++)
	{
		shaderPrograms->at(i)->BindInt("numberOfPointLights", numPointLights);
		shaderPrograms->at(i)->BindInt("numberOfSpotLights", numSpotLights);
	}

	for (int i = 0; i < m_closestLights.size(); i++)
	{
		if (m_closestLights.at(i)->m_transform == nullptr)
			continue;

		m_closestLights.at(i)->m_lightInfo->BindInfoToShaders(i, m_closestLights.at(i)->m_transform);
	}
}

void Light::OnUpdate()
{
	if (m_transform == nullptr)
	{
		m_transform = Application::GetEntity(m_sEntityID)->GetComponent<Transform>();
		return;
	}

	SpotLight* spotLight = dynamic_cast<SpotLight*>(m_lightInfo);
	DirectionalLight* dirLight = dynamic_cast<DirectionalLight*>(m_lightInfo);

	switch (m_lightType)
	{
	case Directional:
		dirLight->m_direction = glm::normalize(m_transform->m_rotation);
		break;
	}

	float xDis = (Renderer::GetCamera()->m_camTransform->m_position.x - m_transform->m_position.x) * (Renderer::GetCamera()->m_camTransform->m_position.x - m_transform->m_position.x);
	float yDis = (Renderer::GetCamera()->m_camTransform->m_position.y - m_transform->m_position.y) * (Renderer::GetCamera()->m_camTransform->m_position.y - m_transform->m_position.y);
	float zDis = (Renderer::GetCamera()->m_camTransform->m_position.z - m_transform->m_position.z) * (Renderer::GetCamera()->m_camTransform->m_position.z - m_transform->m_position.z);

	m_fDistanceToCam = sqrt(xDis + yDis + zDis);
}