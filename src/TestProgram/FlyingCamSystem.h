#pragma once
#ifndef FLYINGCAMSYSTEM_H_
#define FLYINGCAMSYSTEM_H_

#include <NobleEngine/ECS/System.hpp>
#include "FlyingCam.h"

using namespace NobleCore;
struct FlyingCamSystem : public System<FlyingCam>
{
	void OnUpdate(FlyingCam* comp);
	void UpdateControls(FlyingCam* cam);
	void UpdateCameraRotation(FlyingCam* cam);
};

#endif