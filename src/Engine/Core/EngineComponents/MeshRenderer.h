#pragma once

#include "../ECS/Renderable.hpp"
#include "../EngineResources/Model.h"
#include "../EngineResources/Texture.h"

#include "../ResourceManager.h"

#include "Transform.h"

struct MeshRenderer : public Renderable
{
	std::shared_ptr<Model> m_model = nullptr;
	glm::vec4 m_colour = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	std::string GetComponentID() override
	{
		return "MeshRenderer";
	}

	nlohmann::json WriteJson()
	{
		nlohmann::json data;

		if (m_texture != nullptr)
			data["texturePath"] = m_texture->m_sLocalPath;
		if(m_model != nullptr)
			data["modelPath"] = m_model->m_sLocalPath;
		if (m_pipeline != nullptr)
			data["pipeline"] = m_pipeline->m_sLocalPath;

		data["colour"] = { m_colour.x, m_colour.y, m_colour.z, m_colour.w };

		return data;
	}

	void FromJson(const nlohmann::json& j)
	{
		if (j.find("texturePath") != j.end())
			m_texture = ResourceManager::LoadResource<Texture>(j["texturePath"]);
		if (j.find("modelPath") != j.end())
			m_model = ResourceManager::LoadResource<Model>(j["modelPath"]);
		if (j.find("pipeline") != j.end())
			m_pipeline = ResourceManager::LoadResource<Pipeline>(j["pipeline"]);
		if (j.find("colour") != j.end())
			m_colour = glm::vec4(j["colour"][0], j["colour"][1], j["colour"][2], j["colour"][3]);
	}

	void ChangeModel(std::shared_ptr<Model> model)
	{
		if (model == nullptr)
			return;

		if (m_model != nullptr && m_model->m_sLocalPath == model->m_sLocalPath)
			return;

		m_model = model;
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

		ChangeModel(ResourceManager::DoResourceSelectInterface<Model>("Model", m_model != nullptr ? m_model->m_sLocalPath : "none"));
		ChangeTexture(ResourceManager::DoResourceSelectInterface<Texture>("Texture", m_texture != nullptr ? m_texture->m_sLocalPath : "none"));
		ChangePipeline(ResourceManager::DoResourceSelectInterface<Pipeline>("Pipeline", m_pipeline != nullptr ? m_pipeline->m_sLocalPath : "none"));

		ImVec4 color = ImVec4(m_colour.x, m_colour.y, m_colour.z, m_colour.w);
		ImGui::ColorEdit4("Colour", (float*)&color);
		m_colour = glm::vec4(color.x, color.y, color.z, color.w);
	}

	virtual void OnPreRender() override;
};