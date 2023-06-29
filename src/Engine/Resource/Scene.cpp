#include "Scene.h"

#include "..\Core\Logger.h"
#include "..\Core\Application.h"
#include "..\Core\SceneManager.h"
#include "..\Useful.h"

#include <fstream>
#include <nlohmann/json.hpp>

void Scene::OnLoad()
{
	std::ifstream sceneFile(m_sResourcePath);
	nlohmann::json data = nlohmann::json::parse(sceneFile);

	sceneFile.close();

	for (int i = 0; i < SceneManager::m_vKeysToCheck.size(); i++)
	{
		auto check = data.find(SceneManager::m_vKeysToCheck.at(i));
		if (check == data.end())
		{
			Logger::LogError(FormatString("Scene file %s is malformed, missing %s information. ", m_sResourcePath.c_str(), SceneManager::m_vKeysToCheck.at(i).c_str()), 1);
			return;
		}
	}

	Application::m_mainShaderProgram->BindFloat("ambientStrength", data["LightingSettings"]["AmbientStrength"]);
	Application::m_mainShaderProgram->BindVector3("ambientColour", glm::vec3(data["LightingSettings"]["AmbientColour"][0], data["LightingSettings"]["AmbientColour"][1], data["LightingSettings"]["AmbientColour"][2]));
	Renderer::SetClearColour(glm::vec3(data["LightingSettings"]["ClearColour"][0], data["LightingSettings"]["ClearColour"][1], data["LightingSettings"]["ClearColour"][2]));

	//parse out JSON here. Should probably check that the file isnt malformed in some way as well.
	if (data.find("Entities") != data.end())
	{
		nlohmann::json entities = data.at("Entities");

		for (auto it : entities.items())
		{
			Application::CreateEntity(it.key(), it.value());
		}
	}
	if (data.find("ComponentData") != data.end())
	{
		nlohmann::json componentData = data.at("ComponentData");

		for (auto it : componentData.items())
		{
			std::shared_ptr<SystemBase> targetSystem = Application::GetSystemFromID(it.key());
			if(targetSystem != nullptr)
				targetSystem->LoadComponentDataFromJson(componentData[it.key()]);
		}
	}
	if (data.find("Behaviours") != data.end())
	{
		//tbd for the future.
		Logger::LogInformation("Found Behaviours information");
	}
}