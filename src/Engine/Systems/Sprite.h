#pragma once

#include "../ECS/Component.hpp"
#include "Transform.h"
#include "../Core/ResourceManager.h"
#include "../Resource/Texture.h"
#include "../Core/Graphics/GraphicsBuffer.h"

#include <glm/glm.hpp>

struct Sprite : public Component
{
	Transform* m_spriteTransform = nullptr;

	bool m_bCreatedDescriptorSets = false;
	std::vector<GraphicsBuffer> m_uniformBuffers;
	std::vector<void*> m_uniformBuffersMapped;
	std::vector<VkDescriptorSet> m_descriptorSets;

	std::shared_ptr<Texture> m_spriteTexture = nullptr;
	glm::vec4 m_colour;

	nlohmann::json WriteJson()
	{ 
		nlohmann::json data;
		data = { {"spritePath", m_spriteTexture->m_sLocalPath }, {"colour", {m_colour.x, m_colour.y, m_colour.z, m_colour.w}} };

		return data;
	}

	void FromJson(const nlohmann::json& j)
	{
		m_spriteTexture = ResourceManager::LoadResource<Texture>(j["spritePath"]);
		m_colour = glm::vec4(j["colour"][0], j["colour"][1], j["colour"][2], j["colour"][3]);
	}

	void OnInitialize()
	{
		m_colour = glm::vec4(1.0f);
	}

	void OnInitialize(glm::vec4 _colour)
	{
		m_colour = _colour;
	}

	void OnInitialize(std::string _sheetPath, glm::vec4 _colour)
	{
		m_spriteTexture = ResourceManager::LoadResource<Texture>(_sheetPath);
		m_colour = _colour;
	}

	void OnInitialize(std::string _sheetPath)
	{
		m_spriteTexture = ResourceManager::LoadResource<Texture>(_sheetPath);
		m_colour = glm::vec4(1.0f);
	}
};
