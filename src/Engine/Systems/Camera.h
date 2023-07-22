#pragma once
#ifndef CAMERA_H_
#define CAMERA_H_

#include "../ECS/Component.hpp"
#include "Transform.h"

enum CameraState
{
	inactive,
	mainCam,
	editorCam
};

struct Camera : ComponentData<Camera>
{
	Transform* m_camTransform = nullptr;
	CameraState m_state = inactive;

	nlohmann::json WriteJson() override
	{
		nlohmann::json data = { {"CameraState", m_state} };
		return data;
	}

	void FromJson(const nlohmann::json& j) override
	{
		m_state = j["CameraState"];
	}
};

#endif