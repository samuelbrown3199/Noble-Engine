#include "SceneManager.h"

#include "Application.h"
#include "ResourceManager.h"
#include "..\Resource\Scene.h"

void SceneManager::ClearLoadedScene()
{
	Application::ClearLoadedScene();
}

void SceneManager::LoadScene(std::string scenePath)
{
	Application::ClearLoadedScene();
	std::shared_ptr<Scene> newScene = ResourceManager::LoadResource<Scene>(scenePath);
}

void SceneManager::SaveScene(std::string scenePath)
{
	Logger::LogInformation("Saving Scene " + scenePath);

	std::vector<Entity>& entities = Application::GetEntityList();
	std::vector<std::shared_ptr<SystemBase>> systems = Application::GetSystemList();

	nlohmann::json data;

	for (int i = 0; i < entities.size(); i++)
	{
		data["Entities"][entities.at(i).m_sEntityID] = 0;
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