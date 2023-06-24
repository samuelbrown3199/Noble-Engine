#pragma once
#ifndef SCENEMANAGER_H_
#define SCENEMANAGER_H_

#include <string>

class SceneManager
{
public:

	static void ClearLoadedScene();

	static void LoadScene(std::string scenePath);
	static void SaveScene(std::string scenePath);
};

#endif