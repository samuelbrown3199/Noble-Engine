#pragma once

#include <vector>
#include <string>
#include <memory>

#include "ECS/Entity.hpp"

class Scene;

class SceneManager
{
	std::shared_ptr<Scene> m_currentScene;
	std::vector<std::string> m_vScenes;

public:

	std::vector<std::string> m_vKeysToCheck;

	void ClearLoadedScene();

	void CreateNewScene(std::string sceneName);
	void SaveLoadedScene();
	void LoadScene(int sceneIndex);
	void SaveScene(std::string scenePath);

	void LoadDefaultScene();

	void LoadSceneDatabase(nlohmann::json sceneDatabase);
	nlohmann::json WriteSceneDatabase();

	std::string GetCurrentSceneLocalPath();
	std::vector<std::string> GetSceneList() { return m_vScenes; }
};