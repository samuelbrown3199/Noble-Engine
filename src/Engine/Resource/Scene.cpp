#include "Scene.h"

#include "..\Core\Registry.h"
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
	m_sceneData.clear();

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
	if (m_sceneData.find("ComponentData") != m_sceneData.end())
	{
		std::map<int, std::pair<std::string, ComponentRegistry>>* compRegistry = NobleRegistry::GetComponentRegistry();

		nlohmann::json componentData = m_sceneData.at("ComponentData");

		for (auto it : componentData.items())
		{
			for (int i = 0; i < compRegistry->size(); i++)
			{
				if (compRegistry->at(i).first == it.key())
				{
					compRegistry->at(i).second.m_comp->LoadComponentDataFromJson(componentData[it.key()]);
				}
			}
		}
	}
	if (m_sceneData.find("Entities") != m_sceneData.end())
	{
		nlohmann::json entities = m_sceneData.at("Entities");

		for (auto it : entities.items())
		{
			Entity* ent = Application::CreateEntity(it.key(), it.value());
			ent->GetAllComponents();
		}
	}
	if (m_sceneData.find("Behaviours") != m_sceneData.end())
	{
		std::map<int, std::pair<std::string, Behaviour*>>* behRegistry = NobleRegistry::GetBehaviourRegistry();

		nlohmann::json behaviours = m_sceneData.at("Behaviours");

		for (auto it : behaviours.items())
		{
			nlohmann::json entity = behaviours.at(it.key());

			for (auto beh : entity.items())
			{
				for (int o = 0; o < behRegistry->size(); o++)
				{
					if (behRegistry->at(o).first == beh.key())
					{
						behRegistry->at(o).second->LoadBehaviourFromJson(it.key(), beh.value());

						continue;
					}
				}
			}
		}
	}

	if (m_sceneData.find("LightingSettings") != m_sceneData.end())
	{
		nlohmann::json lightingInfo = m_sceneData.at("LightingSettings");

		glm::vec3 clearColour = glm::vec3(lightingInfo["ClearColour"][0], lightingInfo["ClearColour"][1], lightingInfo["ClearColour"][2]);
		Renderer::SetClearColour(clearColour);

		glm::vec3 ambientColour = glm::vec3(lightingInfo["AmbientColour"][0], lightingInfo["AmbientColour"][1], lightingInfo["AmbientColour"][2]);
		Renderer::SetAmbientColour(ambientColour, lightingInfo["AmbientStrength"]);
	}
}