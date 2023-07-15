#pragma once
#ifndef MODEL_H_
#define MODEL_H_

#include <array>
#include <vector>

#include "Resource.h"
#include "..\Core\Graphics\Renderer.h"

struct Model : public Resource
{
	std::vector<Vertex> m_vertices;
	std::vector<uint32_t> m_indices;

	VkBuffer m_vertexBuffer;
	VkDeviceMemory m_vertexBufferMemory;
	VkBuffer m_indexBuffer;
	VkDeviceMemory m_indexBufferMemory;

	void OnLoad();
};

#endif