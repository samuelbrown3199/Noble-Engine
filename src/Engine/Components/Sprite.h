#pragma once

#include "../ECS/Renderable.hpp"
#include "../Core/ResourceManager.h"
#include "../Resource/Texture.h"

#include <GL/glew.h>
#include <glm/glm.hpp>

struct Sprite : public Renderable
{
	std::shared_ptr<Texture> m_spriteTexture = nullptr;
	glm::vec4 m_colour;
	std::shared_ptr<ShaderProgram> m_shader;

	static GLuint m_iQuadVAO;
	static bool m_bInitializedSpriteQuad;
	static std::vector<Vertex> vertices;
	static std::vector<glm::vec3> boundingBox;

	std::string GetComponentID() override
	{
		return "Sprite";
	}

	nlohmann::json WriteJson()
	{ 
		nlohmann::json data;

		if (m_spriteTexture != nullptr)
			data["spritePath"] = m_spriteTexture->m_sLocalPath;
		if (m_shader != nullptr)
			data["ShaderProgram"] = m_shader->m_shaderProgramID;

		data["colour"] = { m_colour.x, m_colour.y, m_colour.z, m_colour.w };

		return data;
	}

	void FromJson(const nlohmann::json& j)
	{
		if(j.find("spritePath") != j.end())
			m_spriteTexture = ResourceManager::LoadResource<Texture>(j["spritePath"]);
		if(j.find("colour") != j.end())
			m_colour = glm::vec4(j["colour"][0], j["colour"][1], j["colour"][2], j["colour"][3]);
		if (j.find("ShaderProgram") != j.end())
			m_shader = ResourceManager::GetShaderProgram(j["ShaderProgram"]);
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

	void ChangeSprite(std::shared_ptr<Texture> sprite)
	{
		if (sprite == nullptr)
			return;

		if (m_spriteTexture != nullptr && sprite->m_sLocalPath == m_spriteTexture->m_sLocalPath)
			return;

		m_spriteTexture = sprite;
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
		if (m_transformIndex == -1)
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

		ChangeSprite(ResourceManager::DoResourceSelectInterface<Texture>("Sprite", m_spriteTexture != nullptr ? m_spriteTexture->m_sLocalPath : "none"));
		ChangeShaderProgram(ResourceManager::DoShaderProgramSelectInterface(m_shader != nullptr ? m_shader->m_shaderProgramID : ""));

		ImVec4 color = ImVec4(m_colour.x, m_colour.y, m_colour.z, m_colour.w);
		ImGui::ColorEdit4("Colour", (float*)&color);
		m_colour = glm::vec4(color.x, color.y, color.z, color.w);
	}


	virtual void PreRender() override;
	virtual void OnRender() override;
};
