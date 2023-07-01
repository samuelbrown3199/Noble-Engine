#pragma once
#ifndef SCENE_H_
#define SCENE_H_

#include <nlohmann/json.hpp>

#include "Resource.h"

class Scene : public Resource
{
	nlohmann::json m_sceneData;
	bool m_bSceneReady = false;

public:
	void OnLoad();
	void LoadSceneIntoApplication();
};

#endif