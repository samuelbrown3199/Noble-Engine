#pragma once
#ifndef MODEL_H_
#define MODEL_H_

#include <array>
#include <vector>

#include "Resource.h"
#include "..\Core\Graphics\Renderer.h"

struct Model : public Resource
{
	size_t m_drawCount;

	void OnLoad();
};

#endif