#pragma once
#ifndef MODEL_H_
#define MODEL_H_

#include <array>
#include <vector>

#include "Resource.h"
#include "..\..\Core\Graphics\Renderer.h"

struct Model : public Resource
{
	std::vector<Vertex> m_vertices;
	std::vector<uint32_t> m_indices;

	GPUMeshBuffers m_meshBuffer;

	std::vector<glm::vec3> m_modelBoundingBox;

	Model();
	~Model();

	void OnLoad() override;
	void OnUnload() override;

	virtual void AddResource(std::string path) override;

	std::shared_ptr<Resource> LoadFromJson(const std::string& path, const nlohmann::json& data) override;

	float GetResourceSize() override;
};

#endif