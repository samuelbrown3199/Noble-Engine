#pragma once
#ifndef MODEL_H_
#define MODEL_H_

#include "Resource.h"

struct Model : public Resource
{
	size_t m_drawCount;

	void OnLoad();
};

#endif