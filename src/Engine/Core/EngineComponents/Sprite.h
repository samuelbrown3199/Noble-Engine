#pragma once

#include "../ECS/Renderable.hpp"
#include "../ECS/ComponentList.hpp"
#include "Transform.h"
#include "../ResourceManager.h"
#include "../EngineResources/Texture.h"

#include <glm/glm.hpp>

struct Sprite : public Renderable
{
	glm::vec4 m_colour = glm::vec4();

	static std::vector<uint32_t> spriteQuadIndices;
	static std::vector<Vertex> spriteQuadVertices;
	static std::vector<glm::vec3> boundingBox;

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
		if(j.find("spritePath") != j.end())
			m_texture = ResourceManager::LoadResource<Texture>(j["spritePath"]);
		if (j.find("pipeline") != j.end())
			m_pipeline = ResourceManager::LoadResource<Pipeline>(j["pipeline"]);
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

	void OnInitialize(std::string _sheetPath, glm::vec4 _colour)
	{
		m_texture = ResourceManager::LoadResource<Texture>(_sheetPath);
		m_colour = _colour;
	}

	void OnInitialize(std::string _sheetPath)
	{
		m_texture = ResourceManager::LoadResource<Texture>(_sheetPath);
		m_colour = glm::vec4(1.0f);
	}

	virtual void DoComponentInterface() override
	{
		if (m_transformIndex == -1)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
			ImGui::Text("No transform attached. Object won't render.");
			ImGui::PopStyleColor();

			ImGui::Dummy(ImVec2(0.0f, 5.0f));
		}

		ImGui::BeginDisabled();
		ImGui::Checkbox("On Screen", &m_bOnScreen);
		ImGui::EndDisabled();

		ChangeTexture(ResourceManager::DoResourceSelectInterface<Texture>("Sprite", m_texture != nullptr ? m_texture->m_sLocalPath : "none"));
		ChangePipeline(ResourceManager::DoResourceSelectInterface<Pipeline>("Pipeline", m_pipeline != nullptr ? m_pipeline->m_sLocalPath : "none"));

		ImVec4 color = ImVec4(m_colour.x, m_colour.y, m_colour.z, m_colour.w);
		ImGui::ColorEdit4("Colour", (float*)&color);
		m_colour = glm::vec4(color.x, color.y, color.z, color.w);
	}

	virtual void OnPreRender() override;
};
