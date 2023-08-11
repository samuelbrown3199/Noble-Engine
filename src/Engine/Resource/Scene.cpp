#include "Scene.h"

#include "..\Core\Logger.h"
#include "..\Core\Application.h"
#include "..\Core\SceneManager.h"
#include "..\Useful.h"

#include <fstream>

Scene::Scene(std::string path)
{
	m_sResourcePath = GetGameFolder() + path;
}

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

	if (m_sceneData.find("LightingSettings") != m_sceneData.end())
	{
		nlohmann::json lightingInfo = m_sceneData.at("LightingSettings");

		glm::vec3 clearColour = glm::vec3(lightingInfo["ClearColour"][0], lightingInfo["ClearColour"][1], lightingInfo["ClearColour"][2]);
		Renderer::SetClearColour(clearColour);
	}
}