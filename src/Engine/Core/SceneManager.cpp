#include "SceneManager.h"

#include "Application.h"
#include "ResourceManager.h"
#include "..\Resource\Scene.h"

std::shared_ptr<Scene> SceneManager::m_currentScene;
std::vector<std::string> SceneManager::m_vKeysToCheck;

void SceneManager::ClearLoadedScene()
{
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
	SaveScene(m_currentScene->GetResourcePath());
}

void SceneManager::SaveScene(std::string scenePath)
{
	Logger::LogInformation("Saving Scene " + scenePath);

	std::vector<Entity>& entities = Application::GetEntityList();
	std::vector<std::shared_ptr<SystemBase>> systems = Application::GetSystemList();

	nlohmann::json data;

	data["LightingSettings"]["AmbientColour"] = { 1,1,1 };
	data["LightingSettings"]["AmbientStrength"] = 1.0f;
	data["LightingSettings"]["ClearColour"] = { 0.0f, 0.0f, 0.0f };

	for (int i = 0; i < entities.size(); i++)
	{
		if (entities.at(i).m_bAvailableForUse)
			continue;

		data["Entities"][entities.at(i).m_sEntityID] = entities.at(i).m_sEntityName;
	}

	for (int i = 0; i < systems.size(); i++)
	{
		data["ComponentData"][systems.at(i)->m_systemID] = systems.at(i)->WriteComponentDataToJson();
	}

	std::fstream sceneFile(scenePath, 'w');
	sceneFile << data.dump();
	sceneFile.close();

	Logger::LogInformation("Saved Scene " + scenePath);
}

std::string SceneManager::GetCurrentSceneLocalPath()
{
	if (m_currentScene)
		return GetFolderLocationRelativeToGameData(m_currentScene->GetResourcePath());
	else
		return "";
}