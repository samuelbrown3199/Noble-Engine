#pragma once
#ifndef CAMERA_H_
#define CAMERA_H_

#include "../ECS/Component.hpp"
#include "Transform.h"

struct Camera : ComponentData<Camera>
{
	Transform* m_camTransform = nullptr;
	bool m_bIsMainCam = false;

	nlohmann::json WriteJson() override
	{
		nlohmann::json data = { {"MainCam", m_bIsMainCam} };
		return data;
	}

	void FromJson(const nlohmann::json& j) override
	{
		m_bIsMainCam = j["MainCam"];
	}
};

#endif