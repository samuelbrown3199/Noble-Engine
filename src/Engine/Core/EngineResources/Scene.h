#pragma once
#ifndef SCENE_H_
#define SCENE_H_

#include <nlohmann/json.hpp>

#include "Resource.h"

class Scene
{
	std::string m_sResourcePath;

	nlohmann::json m_sceneData;
	bool m_bSceneReady = false;

public:

	Scene(std::string path);

	void OnLoad();
	void LoadSceneIntoApplication();
	std::string GetResourcePath() { return m_sResourcePath; }
};

#endif