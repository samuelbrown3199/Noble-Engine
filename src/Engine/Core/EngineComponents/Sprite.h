#pragma once

#include "../ECS/Renderable.hpp"
#include "../ECS/ComponentList.hpp"
#include "Transform.h"
#include "../ResourceManager.h"
#include "../EngineResources/Texture.h"

#include <glm/glm.hpp>

struct Sprite : public Renderable
{
	static std::vector<uint32_t> spriteQuadIndices;
	static std::vector<Vertex> spriteQuadVertices;
	static std::vector<glm::vec3> boundingBox;

	Sprite& operator=(const Sprite& other)
	{
		m_transformIndex = -1;

		m_sTargetTexturePath = other.m_sTargetTexturePath;
		m_texture = nullptr;

		m_sTargetPipelinePath = other.m_sTargetPipelinePath;
		m_pipeline = nullptr;

		m_colour = other.m_colour;

		return *this;
	}

	std::string GetComponentID() override
	{
		return "Sprite";
	}

	nlohmann::json WriteJson()
	{ 
		nlohmann::json data;

		if (m_texture != nullptr)
			data["spritePath"] = m_texture->m_sLocalPath;
		if (m_pipeline != nullptr)
			data["pipeline"] = m_pipeline->m_sLocalPath;

		data["colour"] = { m_colour.x, m_colour.y, m_colour.z, m_colour.w };

		return data;
	}

	void FromJson(const nlohmann::json& j)
	{
		ResourceManager* rManager = Application::GetApplication()->GetResourceManager();

		if (j.find("spritePath") != j.end())
			m_sTargetTexturePath = j["spritePath"];
		if (j.find("pipeline") != j.end())
			m_sTargetPipelinePath = j["pipeline"];
		if(j.find("colour") != j.end())
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

	virtual void DoComponentInterface() override;
	virtual void OnPreRender() override;
};
