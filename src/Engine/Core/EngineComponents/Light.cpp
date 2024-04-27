#include "Light.h"

#include "../Application.h"
#include "../Registry.h"
#include "../CommandTypes.h"

void LightInfo::DoLightInfoInterface()
{
	float diffuse[3] = { m_diffuse.x, m_diffuse.y, m_diffuse.z };
	if (ImGui::ColorEdit3("Diffuse Colour", (float*)&diffuse))
	{
		ChangeValueCommand<glm::vec3>* command = new ChangeValueCommand<glm::vec3>(&m_diffuse, glm::vec3(diffuse[0], diffuse[1], diffuse[2]));
		Application::GetApplication()->PushCommand(command);
	}

	float specular[3] = { m_specular.x, m_specular.y, m_specular.z };
	if (ImGui::ColorEdit3("Specular Colour", (float*)&specular))
	{
		ChangeValueCommand<glm::vec3>* command = new ChangeValueCommand<glm::vec3>(&m_specular, glm::vec3(specular[0], specular[1], specular[2]));
		Application::GetApplication()->PushCommand(command);
	}
};

void DirectionalLight::DoLightInfoInterface()
{
	ImGui::Text("Directional Light");
	ImGui::Dummy(ImVec2(0.0f, 3.0f));

	ImGui::BeginDisabled();
	float direction[3] = { m_direction.x, m_direction.y, m_direction.z };
	ImGui::DragFloat3("Direction", direction, 0.01f, -1, 1, "%.2f");
	ImGui::EndDisabled();

	float intensity = m_fIntensity;
	if (ImGui::DragFloat("Intensity", &intensity, 0.01f, 0.0f, 1.0f, "%.2f"))
	{
		ChangeValueCommand<float>* command = new ChangeValueCommand<float>(&m_fIntensity, intensity);
		Application::GetApplication()->PushCommand(command);
	}

	LightInfo::DoLightInfoInterface();
}

void PointLight::DoLightInfoInterface()
{
	ImGui::Text("Point Light");
	ImGui::Dummy(ImVec2(0.0f, 3.0f));

	ImGui::BeginDisabled();
	ImGui::DragFloat("Constant", &m_constant, 1.0f, 1.0f, 1.0f, "%.2f");
	ImGui::EndDisabled();

	float linear = m_linear;
	if (ImGui::DragFloat("Linear", &linear, 0.01f, 0.0014f, 0.7f, "%.4f"))
	{
		ChangeValueCommand<float>* command = new ChangeValueCommand<float>(&m_linear, linear);
		Application::GetApplication()->PushCommand(command);
	}
	float quadratic = m_quadratic;
	if (ImGui::DragFloat("Quadratic", &quadratic, 0.01f, 0.000007f, 1.8f, "%.6f"))
	{
		ChangeValueCommand<float>* command = new ChangeValueCommand<float>(&m_quadratic, quadratic);
		Application::GetApplication()->PushCommand(command);
	}

	LightInfo::DoLightInfoInterface();
}

void SpotLight::DoLightInfoInterface()
{
	ImGui::Text("Spot Light");
	ImGui::Dummy(ImVec2(0.0f, 3.0f));

	ImGui::BeginDisabled();
	ImGui::DragFloat("Constant", &m_constant, 1.0f, 1.0f, 1.0f, "%.2f");
	ImGui::EndDisabled();

	float linear = m_linear;
	if (ImGui::DragFloat("Linear", &linear, 0.01f, 0.0014f, 0.7f, "%.4f"))
	{
		ChangeValueCommand<float>* command = new ChangeValueCommand<float>(&m_linear, linear);
		Application::GetApplication()->PushCommand(command);
	}

	float quadratic = m_quadratic;
	if (ImGui::DragFloat("Quadratic", &m_quadratic, 0.01f, 0.000007f, 1.8f, "%.6f"))
	{
		ChangeValueCommand<float>* command = new ChangeValueCommand<float>(&m_quadratic, quadratic);
		Application::GetApplication()->PushCommand(command);
	}

	float cutOff = m_fCutOff;
	if (ImGui::DragFloat("Cutoff", &m_fCutOff))
	{
		ChangeValueCommand<float>* command = new ChangeValueCommand<float>(&m_fCutOff, cutOff);
		Application::GetApplication()->PushCommand(command);
	}

	float outerCutOff = m_fOuterCutOff;
	if (ImGui::DragFloat("Outer Cutoff", &m_fOuterCutOff))
	{
		ChangeValueCommand<float>* command = new ChangeValueCommand<float>(&m_fOuterCutOff, outerCutOff);
		Application::GetApplication()->PushCommand(command);
	}

	LightInfo::DoLightInfoInterface();
}




bool SortByDistance(Light* val1, Light* val2)
{
	return val1->m_fDistanceToCam < val2->m_fDistanceToCam;
}

void Light::PreUpdate() //probably a far better way to do this. Legacy code begging for update.
{
	std::vector<Light*> closestPointLights;
	std::vector<Light*> closestSpotLights;
	std::vector<Light*> closestDirLights;

	int numPointLights = 0;
	int numSpotLights = 0;
	int numDirLights = 0;

	NobleRegistry* registry = Application::GetApplication()->GetRegistry();
	ComponentDatalist<Light>* dataList = dynamic_cast<ComponentDatalist<Light>*>(registry->GetComponentList(GetComponentID()));

	for (int i = 0; i < dataList->m_componentData.size(); i++)
	{
		if (dataList->m_componentData.at(i).m_bAvailableForReuse)
			continue;

		if (closestDirLights.size() < 4)
		{
			if (dataList->m_componentData.at(i).m_lightType == Directional)
			{
				closestDirLights.push_back(&dataList->m_componentData.at(i));
				std::sort(closestDirLights.begin(), closestDirLights.end(), SortByDistance);
			}
		}
		else
		{
			Light* curLight = &dataList->m_componentData.at(i);
			if (dataList->m_componentData.at(i).m_lightType == Directional)
			{
				if (curLight->m_fDistanceToCam < closestDirLights.back()->m_fDistanceToCam)
				{
					closestDirLights.pop_back();
					closestDirLights.push_back(&dataList->m_componentData.at(i));
					std::sort(closestDirLights.begin(), closestDirLights.end(), SortByDistance);
				}
			}
		}

		if (closestPointLights.size() < 64)
		{
			if (dataList->m_componentData.at(i).m_lightType == Point)
			{
				closestPointLights.push_back(&dataList->m_componentData.at(i));
				std::sort(closestPointLights.begin(), closestPointLights.end(), SortByDistance);
			}
			else if (dataList->m_componentData.at(i).m_lightType == Spot)
			{
				closestSpotLights.push_back(&dataList->m_componentData.at(i));
				std::sort(closestSpotLights.begin(), closestSpotLights.end(), SortByDistance);
			}

		}
		else
		{
			Light* curLight = &dataList->m_componentData.at(i);
			if (dataList->m_componentData.at(i).m_lightType == Point)
			{
				if (curLight->m_fDistanceToCam < closestPointLights.back()->m_fDistanceToCam)
				{
					closestPointLights.pop_back();
					closestPointLights.push_back(&dataList->m_componentData.at(i));
					std::sort(closestPointLights.begin(), closestPointLights.end(), SortByDistance);
				}
				else if (curLight->m_fDistanceToCam < closestSpotLights.back()->m_fDistanceToCam)
				{
					closestSpotLights.pop_back();
					closestSpotLights.push_back(&dataList->m_componentData.at(i));
					std::sort(closestSpotLights.begin(), closestSpotLights.end(), SortByDistance);
				}
			}
		}
	}

	Renderer* renderer = Application::GetApplication()->GetRenderer();

	for (int i = 0; i < closestPointLights.size(); i++)
	{
		NobleRegistry* registry = Application::GetApplication()->GetRegistry();
		Transform* transform = registry->GetComponent<Transform>(closestPointLights.at(i)->m_transformIndex);
		if (transform == nullptr)
			continue;

		//Put information into renderer scene data.
		PointLight* pointLight = dynamic_cast<PointLight*>(closestPointLights.at(i)->m_lightInfo);
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
	renderer->m_sceneData.numberOfPointLights = closestPointLights.size();

	for(int i = 0; i < closestDirLights.size(); i++)
	{
		Transform* transform = registry->GetComponent<Transform>(closestDirLights.at(i)->m_transformIndex);
		if (transform == nullptr)
			return;

		//Put information into renderer scene data.
		DirectionalLight* dirLight = dynamic_cast<DirectionalLight*>(closestDirLights.at(i)->m_lightInfo);
		if (dirLight != nullptr)
		{
			dirLight->m_direction = transform->m_rotation;

			renderer->m_sceneData.directionalLights[i].direction = dirLight->m_direction;
			renderer->m_sceneData.directionalLights[i].diffuseLight = dirLight->m_diffuse;
			renderer->m_sceneData.directionalLights[i].specularLight = dirLight->m_specular;
			renderer->m_sceneData.directionalLights[i].intensity = dirLight->m_fIntensity;
		}
	}
	renderer->m_sceneData.numberOfDirLights = closestDirLights.size();
}

void Light::OnUpdate()
{
	Renderer* renderer = Application::GetApplication()->GetRenderer();
	NobleRegistry* registry = Application::GetApplication()->GetRegistry();
	if (m_transformIndex == -1)
	{
		m_transformIndex = registry->GetComponentIndex<Transform>(m_sEntityID);
		return;
	}

	SpotLight* spotLight = dynamic_cast<SpotLight*>(m_lightInfo);
	DirectionalLight* dirLight = dynamic_cast<DirectionalLight*>(m_lightInfo);

	Transform* transform = registry->GetComponent<Transform>(m_transformIndex);
	Transform* camTransform = nullptr;
	if (renderer->GetCamera() != nullptr)
	{
		camTransform = registry->GetComponent<Transform>(renderer->GetCamera()->m_camTransformIndex);

		if (camTransform == nullptr)
			return;
	}
	else
	{
		return;
	}

	switch (m_lightType)
	{
	case Directional:
		if(dirLight == nullptr)
			return;
		dirLight->m_direction = glm::normalize(transform->m_rotation);
		break;
	}

	float xDis = (camTransform->m_position.x - transform->m_position.x) * (camTransform->m_position.x - transform->m_position.x);
	float yDis = (camTransform->m_position.y - transform->m_position.y) * (camTransform->m_position.y - transform->m_position.y);
	float zDis = (camTransform->m_position.z - transform->m_position.z) * (camTransform->m_position.z - transform->m_position.z);

	m_fDistanceToCam = sqrt(xDis + yDis + zDis);
}

void Light::DoComponentInterface()
{
	if (m_transformIndex == -1)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
		ImGui::Text("No transform attached. Light won't render.");
		ImGui::PopStyleColor();

		ImGui::Dummy(ImVec2(0.0f, 5.0f));
	}

	const char* lightTypes[] = { "Point", "Spot NYI", "Directional" };
	int selLightType = m_lightType;
	if (ImGui::Combo("Light Type", &selLightType, lightTypes, IM_ARRAYSIZE(lightTypes)))
	{
		ChangeValueCommand<LightType>* command = new ChangeValueCommand<LightType>(&m_lightType, (LightType)selLightType);
		Application::GetApplication()->PushCommand(command);
	}
	ChangeLightType((LightType)selLightType);
	ImGui::Dummy(ImVec2(0.0f, 5.0f));

	ImGui::Text("Light Information");
	if (m_lightInfo)
		m_lightInfo->DoLightInfoInterface();
}