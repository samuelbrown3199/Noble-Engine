#pragma once
#ifndef FLYINGCAM_H_
#define FLYINGCAM_H_


#include <NobleEngine/ECS/Component.hpp>

#include <glm/glm.hpp>

using namespace NobleCore;
struct FlyingCam : public ComponentData<FlyingCam>
{
	bool firstMouse = true;
	float pitch = 0.0f, yaw = -90.0f;

	float movementSpeed = 0.5f;
	glm::vec2 oldMousePos, newMousePos;
};

#endif