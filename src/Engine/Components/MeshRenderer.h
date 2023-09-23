#pragma once

#include "../ECS/ComponentList.hpp"
#include "../ECS/Renderable.hpp"
#include "../Resource/Model.h"
#include "../Resource/Texture.h"

#include "../Core/ResourceManager.h"

#include "Transform.h"

struct MeshRenderer : public Renderable
{
	std::shared_ptr<Model> m_model = nullptr;
	std::shared_ptr<Texture> m_texture = nullptr;

	glm::vec4 m_colour = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	std::shared_ptr<ShaderProgram> m_shader;

	static ComponentDatalist<MeshRenderer> m_componentList;

	nlohmann::json WriteJson()
	{
		nlohmann::json data;

		if (m_texture != nullptr)
			data["texturePath"] = m_texture->m_sLocalPath;
		if(m_model != nullptr)
			data["modelPath"] = m_model->m_sLocalPath;
		if (m_shader != nullptr)
			data["ShaderProgram"] = m_shader->m_shaderProgramID;

		data["colour"] = { m_colour.x, m_colour.y, m_colour.z, m_colour.w };

		return data;
	}

	void FromJson(const nlohmann::json& j)
	{
		if (j.find("texturePath") != j.end())
			m_texture = ResourceManager::LoadResource<Texture>(j["texturePath"]);
		if (j.find("modelPath") != j.end())
			m_model = ResourceManager::LoadResource<Model>(j["modelPath"]);
		if (j.find("colour") != j.end())
			m_colour = glm::vec4(j["colour"][0], j["colour"][1], j["colour"][2], j["colour"][3]);
		if (j.find("ShaderProgram") != j.end())
			m_shader = ResourceManager::GetShaderProgram(j["ShaderProgram"]);
	}

	Component* GetAsComponent(std::string entityID) override
	{
		return GetComponent(entityID);
	}

	void ChangeTexture(std::shared_ptr<Texture> texture)
	{
		if (texture == nullptr)
			return;

		if (m_texture != nullptr && texture->m_sLocalPath == m_texture->m_sLocalPath)
			return;

		m_texture = texture;
	}

	void ChangeModel(std::shared_ptr<Model> model)
	{
		if (model == nullptr)
			return;

		if (m_model != nullptr && m_model->m_sLocalPath == model->m_sLocalPath)
			return;

		m_model = model;
	}

	void ChangeShaderProgram(std::shared_ptr<ShaderProgram> sProgram)
	{
		if (sProgram == nullptr)
			return;

		if (m_shader != nullptr && sProgram->m_shaderProgramID == m_shader->m_shaderProgramID)
			return;

		m_shader = sProgram;
	}

	virtual void DoComponentInterface() override
	{
		if (m_transform == nullptr)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
			ImGui::Text("No transform attached. Object won't render.");
			ImGui::PopStyleColor();

			ImGui::Dummy(ImVec2(0.0f, 5.0f));
		}

		if (m_shader == nullptr)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
			ImGui::Text("No shader selected. Object won't render.");
			ImGui::PopStyleColor();

			ImGui::Dummy(ImVec2(0.0f, 5.0f));
		}

		ImGui::BeginDisabled();
		ImGui::Checkbox("On Screen", &m_bOnScreen);
		ImGui::EndDisabled();

		ChangeTexture(ResourceManager::DoResourceSelectInterface<Texture>("Texture", m_texture != nullptr ? m_texture->m_sLocalPath : "none"));
		ChangeModel(ResourceManager::DoResourceSelectInterface<Model>("Model", m_model != nullptr ? m_model->m_sLocalPath : "none"));

		ChangeShaderProgram(ResourceManager::DoShaderProgramSelectInterface(m_shader != nullptr ? m_shader->m_shaderProgramID : ""));

		ImVec4 color = ImVec4(m_colour.x, m_colour.y, m_colour.z, m_colour.w);
		ImGui::ColorEdit4("Colour", (float*)&color);
		m_colour = glm::vec4(color.x, color.y, color.z, color.w);
	}

	virtual void AddComponent() override;
	virtual void AddComponentToEntity(std::string entityID) override;
	virtual void RemoveComponent(std::string entityID) override;
	virtual void RemoveAllComponents() override;

	MeshRenderer* GetComponent(std::string entityID);

	virtual void Update(bool useThreads, int maxComponentsPerThread) override;

	virtual void Render(bool useThreads, int maxComponentsPerThread) override;
	virtual void OnRender() override;

	virtual void LoadComponentDataFromJson(nlohmann::json& j) override;
	virtual nlohmann::json WriteComponentDataToJson() override;
};