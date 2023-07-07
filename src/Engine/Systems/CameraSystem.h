#pragma once
#ifndef CAMERASYSTEM_H_
#define CAMERASYSTEM_H_

#include "../ECS/System.hpp"
#include "Camera.h"

struct CameraSystem : public System<Camera>
{
	void OnUpdate(Camera* comp);
};

#endif