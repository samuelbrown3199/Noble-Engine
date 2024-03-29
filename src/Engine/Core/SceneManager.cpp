#include "SceneManager.h"

#include "Application.h"
#include "Registry.h"
#include "ResourceManager.h"
#include "../Core/EngineResources\Scene.h"

#include "../imgui/imgui.h"

std::shared_ptr<Scene> SceneManager::m_currentScene = nullptr;
std::vector<std::string> SceneManager::m_vKeysToCheck;

void SceneManager::ClearLoadedScene()
{
	m_currentScene = nullptr;
	Application::ClearLoadedScene();
}

void SceneManager::LoadScene(std::string scenePath)
{
	m_vKeysToCheck.clear();
	m_vKeysToCheck.push_back("LightingSettings");
	m_vKeysToCheck.push_back("Entities");
	m_vKeysToCheck.push_back("ComponentData");

	Application::ClearLoadedScene();
	m_currentScene = std::make_shared<Scene>(scenePath);
	m_currentScene->OnLoad();
	m_currentScene->LoadSceneIntoApplication();
}

void SceneManager::SaveLoadedScene()
{
	if (m_currentScene == nullptr)
		return;

	SaveScene(m_currentScene->GetResourcePath());
	m_currentScene->OnLoad();
}

void SceneManager::SaveScene(std::string scenePath)
{
	if (Application::GetPlayMode())
		return;

	if (scenePath.empty())
		return;

	Logger::LogInformation("Saving Scene " + scenePath);

	std::vector<Entity>& entities = Application::GetEntityList();
	std::map<int, std::pair<std::string, ComponentRegistry>>* compRegistry = NobleRegistry::GetComponentRegistry();
	std::map<int, std::pair<std::string, Behaviour*>>* behRegistry = NobleRegistry::GetBehaviourRegistry();

	nlohmann::json data;
	AddVersionDataToFile(data);

	glm::vec3 clearColour = Renderer::GetClearColour();
	data["LightingSettings"]["ClearColour"] = { clearColour.x, clearColour.y, clearColour.z };

	for (int i = 0; i < entities.size(); i++)
	{
		if (entities.at(i).m_bAvailableForUse)
			continue;

		data["Entities"][entities.at(i).m_sEntityID]["EntityName"] = entities.at(i).m_sEntityName;
		data["Entities"][entities.at(i).m_sEntityID]["ParentID"] = entities.at(i).m_sEntityParentID;

		for (int o = 0; o < behRegistry->size(); o++)
		{
			Behaviour* currentBehaviour = behRegistry->at(o).second->GetAsBehaviour(entities.at(i).m_sEntityID);
			if (currentBehaviour != nullptr)
			{
				data["Behaviours"][entities.at(i).m_sEntityID][behRegistry->at(o).first] = currentBehaviour->WriteBehaviourToJson();
			}
		}
	}
	for (int i = 0; i < compRegistry->size(); i++)
	{
		data["ComponentData"][compRegistry->at(i).first] = compRegistry->at(i).second.m_comp->WriteComponentDataToJson();
	}

	std::fstream sceneFile(scenePath, 'w');
	sceneFile << data.dump();
	sceneFile.close();

	Application::LinkChildEntities();

	Logger::LogInformation("Saved Scene " + scenePath);
}

std::string SceneManager::GetCurrentSceneLocalPath()
{
	if (m_currentScene)
		return GetFolderLocationRelativeToGameData(m_currentScene->GetResourcePath());
	else
		return "";
}