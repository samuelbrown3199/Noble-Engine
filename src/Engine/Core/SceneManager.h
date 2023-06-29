#pragma once
#ifndef SCENEMANAGER_H_
#define SCENEMANAGER_H_

#include <vector>
#include <string>

class SceneManager
{
	static std::string m_sLoadedScene;

public:

	static std::vector<std::string> m_vKeysToCheck;

	static void ClearLoadedScene();

	static void SaveLoadedScene();
	static void LoadScene(std::string scenePath);
	static void SaveScene(std::string scenePath);
};

#endif