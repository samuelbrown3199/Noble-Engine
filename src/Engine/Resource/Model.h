#pragma once
#ifndef MODEL_H_
#define MODEL_H_

#include <array>
#include <vector>

#include "Resource.h"
#include "..\Core\Graphics\Renderer.h"
#include "..\Core\Graphics\GraphicsBuffer.h"

struct Model : public Resource
{
	std::vector<Vertex> m_vertices;
	std::vector<uint32_t> m_indices;

	GraphicsBuffer m_vertexBuffer;
	GraphicsBuffer m_indexBuffer;

	~Model();
	void OnLoad();
};

#endif