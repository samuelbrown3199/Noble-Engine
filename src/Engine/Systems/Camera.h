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

enum ViewMode
{
	projection,
	orthographic
};

struct Camera : ComponentData<Camera>
{
	Transform* m_camTransform = nullptr;
	CameraState m_state = inactive;
	ViewMode m_viewMode = orthographic;

	nlohmann::json WriteJson() override
	{
		nlohmann::json data = { {"CameraState", m_state}, {"ViewMode", m_viewMode} };
		return data;
	}

	void FromJson(const nlohmann::json& j) override
	{
		m_state = j["CameraState"];
		m_viewMode = j["ViewMode"];
	}
};

#endif