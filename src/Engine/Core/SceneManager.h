#pragma once
#ifndef SCENEMANAGER_H_
#define SCENEMANAGER_H_

#include <vector>
#include <string>
#include <memory>

#include "ECS/Entity.hpp"

class Scene;

class SceneManager
{
	static std::shared_ptr<Scene> m_currentScene;

public:

	static std::vector<std::string> m_vKeysToCheck;

	static void ClearLoadedScene();

	static void SaveLoadedScene();
	static void LoadScene(std::string scenePath);
	static void SaveScene(std::string scenePath);

	static std::string GetCurrentSceneLocalPath();
};

#endif