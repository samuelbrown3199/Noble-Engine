#include "Light.h"

#include "../Application.h"
#include "../Registry.h"

bool SortByDistance(Light* val1, Light* val2)
{
	return val1->m_fDistanceToCam < val2->m_fDistanceToCam;
}

void Light::PreUpdate() //probably a far better way to do this. Legacy code begging for update.
{
	std::vector<Light*> closestLights;

	int numPointLights = 0;
	int numSpotLights = 0;

	ComponentDatalist<Light>* dataList = dynamic_cast<ComponentDatalist<Light>*>(NobleRegistry::GetComponentList(GetComponentID()));

	for (int i = 0; i < dataList->m_componentData.size(); i++)
	{
		if (dataList->m_componentData.at(i).m_bAvailableForReuse)
			continue;

		if (closestLights.size() < 64)
		{
			closestLights.push_back(&dataList->m_componentData.at(i));
			std::sort(closestLights.begin(), closestLights.end(), SortByDistance);
		}
		else
		{
			Light* curLight = &dataList->m_componentData.at(i);
			if (curLight->m_fDistanceToCam < closestLights.back()->m_fDistanceToCam)
			{
				closestLights.pop_back();
				closestLights.push_back(&dataList->m_componentData.at(i));
				std::sort(closestLights.begin(), closestLights.end(), SortByDistance);
			}
		}
	}

	Renderer* renderer = Application::GetApplication()->GetRenderer();

	for (int i = 0; i < closestLights.size(); i++)
	{
		Transform* transform = NobleRegistry::GetComponent<Transform>(closestLights.at(i)->m_transformIndex);
		if (transform == nullptr)
			continue;

		//Put information into renderer scene data.
		PointLight* pointLight = dynamic_cast<PointLight*>(closestLights.at(i)->m_lightInfo);
		if (pointLight != nullptr)
		{
			renderer->m_sceneData.pointLights[i].position = transform->m_position;
			renderer->m_sceneData.pointLights[i].constant = pointLight->m_constant;
			renderer->m_sceneData.pointLights[i].quadratic = pointLight->m_quadratic;
			renderer->m_sceneData.pointLights[i].linear = pointLight->m_linear;
			renderer->m_sceneData.pointLights[i].diffuseLight = pointLight->m_diffuse;
			renderer->m_sceneData.pointLights[i].specularLight = pointLight->m_specular;
		}
	}
	renderer->m_sceneData.numberOfPointLights = closestLights.size();
}

void Light::OnUpdate()
{
	if (m_transformIndex == -1)
	{
		m_transformIndex = NobleRegistry::GetComponentIndex<Transform>(m_sEntityID);
		return;
	}

	SpotLight* spotLight = dynamic_cast<SpotLight*>(m_lightInfo);
	DirectionalLight* dirLight = dynamic_cast<DirectionalLight*>(m_lightInfo);

	Transform* transform = NobleRegistry::GetComponent<Transform>(m_transformIndex);
	Transform* camTransform = NobleRegistry::GetComponent<Transform>(Renderer::GetCamera()->m_camTransformIndex);

	switch (m_lightType)
	{
	case Directional:
		dirLight->m_direction = glm::normalize(transform->m_rotation);
		break;
	}

	float xDis = (camTransform->m_position.x - transform->m_position.x) * (camTransform->m_position.x - transform->m_position.x);
	float yDis = (camTransform->m_position.y - transform->m_position.y) * (camTransform->m_position.y - transform->m_position.y);
	float zDis = (camTransform->m_position.z - transform->m_position.z) * (camTransform->m_position.z - transform->m_position.z);

	m_fDistanceToCam = sqrt(xDis + yDis + zDis);
}