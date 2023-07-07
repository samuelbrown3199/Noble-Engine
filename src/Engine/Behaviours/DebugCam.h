#pragma once
#ifndef DEBUGCAMSYSTEM_H_
#define DEBUGCAMSYSTEM_H_

#include "../ECS/Behaviour.hpp"
#include "DebugCam.h"

#include <glm/glm.hpp>

struct DebugCam : public Behaviour
{
	float m_fMovementSpeed = 16.0f, sensitivity = 0.1f;
	glm::vec2 newMousePos, oldMousePos;
	bool firstMouse = false;
	float pitch = 0.0f, yaw = -90.0f;

	void Start();
	void Update();
	void UpdateControls();
	void UpdateCameraRotation();
};

#endif