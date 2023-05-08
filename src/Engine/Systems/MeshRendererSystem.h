#pragma once
#ifndef MESHRENDERERSYSTEM_H_
#define MESHRENDERERSYSTEM_H_

#include "../ECS/System.hpp"
#include "MeshRenderer.h"

#include <GL/glew.h>

struct MeshRendererSystem : public System<MeshRenderer>
{
	void OnRender(MeshRenderer* comp);
};

#endif