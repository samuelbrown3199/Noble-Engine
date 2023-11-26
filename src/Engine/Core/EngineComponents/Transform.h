#pragma once

#include "../ECS/Component.h"

#include <glm/glm.hpp>

struct Transform : Component
{
	glm::vec3 m_prevPosition = glm::vec3();
	glm::vec3 m_prevRotation = glm::vec3();
	glm::vec3 m_prevScale = glm::vec3();

	glm::vec3 m_position = glm::vec3();
	glm::vec3 m_rotation = glm::vec3();
	glm::vec3 m_scale = glm::vec3();

	glm::vec3 m_localPosition = glm::vec3();
	glm::vec3 m_localRotation = glm::vec3();
	glm::vec3 m_localScale = glm::vec3();

	glm::mat4 m_transformMat = glm::mat4();

	bool m_bDoneParentCheck = false;
	int m_parentTransformIndex = -1;

	std::string GetComponentID() override
	{
		return "Transform";
	}

	nlohmann::json WriteJson() override
	{
		nlohmann::json data = { {"position",{m_position.x, m_position.y, m_position.z}}, {"rotation",{m_rotation.x, m_rotation.y, m_rotation.z}}, {"scale",{m_scale.x, m_scale.y, m_scale.z}} };
		return data;
	}

	void FromJson(const nlohmann::json& j) override
	{
		m_position = glm::vec3(j.at("position")[0], j.at("position")[1], j.at("position")[2]);
		m_rotation = glm::vec3(j.at("rotation")[0], j.at("rotation")[1], j.at("rotation")[2]);
		m_scale = glm::vec3(j.at("scale")[0], j.at("scale")[1], j.at("scale")[2]);

		m_localPosition = m_position;
		m_localRotation = m_rotation;
		m_localScale = m_scale;
	}

	void OnInitialize()
	{
		m_position = glm::vec3(0, 0, 0);
		m_rotation = glm::vec3(0, 0, 0);
		m_scale = glm::vec3(1, 1, 1);
	}

	void OnInitialize(const glm::vec3& _pos)
	{
		m_position = _pos;
		m_rotation = glm::vec3(0, 0, 0);
		m_scale = glm::vec3(1, 1, 1);
	}

	void OnInitialize(const glm::vec3& _pos, const glm::vec3& _rot) 
	{
		m_position = _pos;
		m_rotation = _rot;
		m_scale = glm::vec3(1, 1, 1);
	}

	void OnInitialize(const glm::vec3& _pos, const glm::vec3& _rot, const glm::vec3& _sca)
	{
		m_position = _pos;
		m_rotation = _rot;
		m_scale = _sca;
	}

	virtual void DoComponentInterface() override
	{
		if (m_parentTransformIndex == -1)
		{
			float position[3] = { m_position.x, m_position.y, m_position.z };
			ImGui::DragFloat3("Position", position, 0.01f);
			m_position = glm::vec3(position[0], position[1], position[2]);

			float rotation[3] = { m_rotation.x, m_rotation.y, m_rotation.z };
			ImGui::DragFloat3("Rotation", rotation, 0.01f);
			m_rotation = glm::vec3(rotation[0], rotation[1], rotation[2]);

			float scale[3] = { m_scale.x, m_scale.y, m_scale.z };
			ImGui::DragFloat3("Scale", scale, 0.01f);
			m_scale = glm::vec3(scale[0], scale[1], scale[2]);
		}
		else
		{
			float position[3] = { m_localPosition.x, m_localPosition.y, m_localPosition.z };
			ImGui::DragFloat3("Local Position", position, 0.01f);
			m_localPosition = glm::vec3(position[0], position[1], position[2]);

			float rotation[3] = { m_localRotation.x, m_localRotation.y, m_localRotation.z };
			ImGui::DragFloat3("Local Rotation", rotation, 0.01f);
			m_localRotation = glm::vec3(rotation[0], rotation[1], rotation[2]);

			float scale[3] = { m_localScale.x, m_localScale.y, m_localScale.z };
			ImGui::DragFloat3("Local Scale", scale, 0.01f);
			m_localScale = glm::vec3(scale[0], scale[1], scale[2]);
		}
	}

	virtual void OnUpdate() override;
};