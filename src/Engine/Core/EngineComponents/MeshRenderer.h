#pragma once

#include "../ECS/Renderable.hpp"
#include "../EngineResources/Model.h"
#include "../EngineResources/Texture.h"

#include "../ResourceManager.h"

#include "Transform.h"

struct MeshRenderer : public Renderable
{
	std::string m_sTargetModelPath = "none";
	std::shared_ptr<Model> m_model = nullptr;

	MeshRenderer& operator=(const MeshRenderer& other)
	{
		m_transformIndex = -1;

		m_sTargetModelPath = other.m_sTargetModelPath;
		m_model = nullptr;

		m_sTargetTexturePath = other.m_sTargetTexturePath;
		m_texture = nullptr;

		m_sTargetPipelinePath = other.m_sTargetPipelinePath;
		m_pipeline = nullptr;

		m_colour = other.m_colour;

		return *this;
	}

	std::string GetComponentID() override
	{
		return "MeshRenderer";
	}

	nlohmann::json WriteJson()
	{
		nlohmann::json data;

		if (m_texture != nullptr)
			data["texturePath"] = m_sTargetTexturePath;
		if(m_model != nullptr)
			data["modelPath"] = m_sTargetModelPath;
		if (m_pipeline != nullptr)
			data["pipeline"] = m_sTargetPipelinePath;

		data["colour"] = { m_colour.x, m_colour.y, m_colour.z, m_colour.w };

		return data;
	}

	void FromJson(const nlohmann::json& j)
	{
		ResourceManager* rManager = Application::GetApplication()->GetResourceManager();

		if (j.find("texturePath") != j.end())
			m_sTargetTexturePath = j["texturePath"];
		if (j.find("modelPath") != j.end())
			m_sTargetModelPath = j["modelPath"];
		if (j.find("pipeline") != j.end())
			m_sTargetPipelinePath = j["pipeline"];
		if (j.find("colour") != j.end())
			m_colour = glm::vec4(j["colour"][0], j["colour"][1], j["colour"][2], j["colour"][3]);
	}

	virtual void DoComponentInterface() override;

	virtual void OnPreRender() override;
};