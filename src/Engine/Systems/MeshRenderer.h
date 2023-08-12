#pragma once

#include "../ECS/Component.hpp"
#include "../Resource/Model.h"
#include "../Resource/Texture.h"

#include "..\Core\ResourceManager.h"

#include "Transform.h"

struct MeshRenderer : public Component
{
	std::shared_ptr<Model> m_model = nullptr;
	std::shared_ptr<Texture> m_texture = nullptr;

	glm::vec4 m_colour = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	Transform* m_transform = nullptr;

	bool m_bCreatedDescriptorSets = false;
	std::vector<GraphicsBuffer> m_uniformBuffers;
	std::vector<void*> m_uniformBuffersMapped;
	std::vector<VkDescriptorSet> m_descriptorSets;

	nlohmann::json WriteJson()
	{
		nlohmann::json data;
		data = { {"texturePath", m_texture->m_sLocalPath }, {"modelPath", m_model->m_sLocalPath},{"colour", {m_colour.x, m_colour.y, m_colour.z, m_colour.w}}};

		return data;
	}

	void FromJson(const nlohmann::json& j)
	{
		m_texture = ResourceManager::LoadResource<Texture>(j["texturePath"]);
		m_model = ResourceManager::LoadResource<Model>(j["modelPath"]);
		m_colour = glm::vec4(j["colour"][0], j["colour"][1], j["colour"][2], j["colour"][3]);
	}
};