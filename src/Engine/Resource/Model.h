#pragma once
#ifndef MODEL_H_
#define MODEL_H_

#include <GL/glew.h>

#include "Resource.h"

struct Model : public Resource
{
	GLuint m_vaoID;
	size_t m_drawCount;

	void OnLoad();
};

#endif