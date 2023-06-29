#include "SceneManager.h"

#include "Application.h"
#include "ResourceManager.h"
#include "..\Resource\Scene.h"

std::string SceneManager::m_sLoadedScene = "";
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
	m_sLoadedScene = scenePath;
	std::shared_ptr<Scene> newScene = ResourceManager::LoadResource<Scene>(m_sLoadedScene);
}

void SceneManager::SaveLoadedScene()
{
	SaveScene(m_sLoadedScene);
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
		data["Entities"][entities.at(i).m_sEntityID] = entities.at(i).m_sEntityName;
	}

	for (int i = 0; i < systems.size(); i++)
	{
		data["ComponentData"][systems.at(i)->m_systemID] = systems.at(i)->WriteComponentDataToJson();
	}

	std::fstream sceneFile(scenePath, 'w');
	sceneFile << data;

	sceneFile.close();
	Logger::LogInformation("Saved Scene " + scenePath);
}