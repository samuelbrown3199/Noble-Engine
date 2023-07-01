#include "Scene.h"

#include "..\Core\Logger.h"
#include "..\Core\Application.h"
#include "..\Core\SceneManager.h"
#include "..\Useful.h"

#include <fstream>

void Scene::OnLoad()
{
	std::ifstream sceneFile(m_sResourcePath);
	m_sceneData = nlohmann::json::parse(sceneFile);

	sceneFile.close();

	for (int i = 0; i < SceneManager::m_vKeysToCheck.size(); i++)
	{
		auto check = m_sceneData.find(SceneManager::m_vKeysToCheck.at(i));
		if (check == m_sceneData.end())
		{
			Logger::LogError(FormatString("Scene file %s is malformed, missing %s information. ", m_sResourcePath.c_str(), SceneManager::m_vKeysToCheck.at(i).c_str()), 1);
			return;
		}
	}

	m_bSceneReady = true;
}

void Scene::LoadSceneIntoApplication()
{
	if (m_bSceneReady == false)
		return;

	Application::m_mainShaderProgram->BindFloat("ambientStrength", m_sceneData["LightingSettings"]["AmbientStrength"]);
	Application::m_mainShaderProgram->BindVector3("ambientColour", glm::vec3(m_sceneData["LightingSettings"]["AmbientColour"][0], m_sceneData["LightingSettings"]["AmbientColour"][1], m_sceneData["LightingSettings"]["AmbientColour"][2]));
	Renderer::SetClearColour(glm::vec3(m_sceneData["LightingSettings"]["ClearColour"][0], m_sceneData["LightingSettings"]["ClearColour"][1], m_sceneData["LightingSettings"]["ClearColour"][2]));

	//parse out JSON here. Should probably check that the file isnt malformed in some way as well.
	if (m_sceneData.find("Entities") != m_sceneData.end())
	{
		nlohmann::json entities = m_sceneData.at("Entities");

		for (auto it : entities.items())
		{
			Application::CreateEntity(it.key(), it.value());
		}
	}
	if (m_sceneData.find("ComponentData") != m_sceneData.end())
	{
		nlohmann::json componentData = m_sceneData.at("ComponentData");

		for (auto it : componentData.items())
		{
			std::shared_ptr<SystemBase> targetSystem = Application::GetSystemFromID(it.key());
			if (targetSystem != nullptr)
				targetSystem->LoadComponentDataFromJson(componentData[it.key()]);
		}
	}
	if (m_sceneData.find("Behaviours") != m_sceneData.end())
	{
		//tbd for the future.
		Logger::LogInformation("Found Behaviours information");
	}
}