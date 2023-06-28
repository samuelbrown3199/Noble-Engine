#include "Scene.h"

#include "..\Core\Logger.h"
#include "..\Core\Application.h"

#include <fstream>
#include <nlohmann/json.hpp>

void Scene::OnLoad()
{
	std::ifstream sceneFile(m_sResourcePath);
	nlohmann::json data = nlohmann::json::parse(sceneFile);

	sceneFile.close();

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