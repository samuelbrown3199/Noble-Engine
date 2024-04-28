#pragma once

#include "../ECS/Component.h"

#include <glm/glm.hpp>

struct Transform : Component
{
	glm::vec3 m_prevPosition = glm::vec3();
	glm::vec3 m_prevRotation = glm::vec3();
	glm::vec3 m_prevScale = glm::vec3();

	glm::vec3 m_position = glm::vec3(0,0,0);
	glm::vec3 m_rotation = glm::vec3(0, 0, 0);
	glm::vec3 m_scale = glm::vec3(1,1,1);

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
		nlohmann::json data;
		if(m_parentTransformIndex == -1)
			data = { {"position",{m_position.x, m_position.y, m_position.z}}, {"rotation",{m_rotation.x, m_rotation.y, m_rotation.z}}, {"scale",{m_scale.x, m_scale.y, m_scale.z}} };
		else
			data = { {"position",{m_localPosition.x, m_localPosition.y, m_localPosition.z}}, {"rotation",{m_localRotation.x, m_localRotation.y, m_localRotation.z}}, {"scale",{m_localScale.x, m_localScale.y, m_localScale.z}} };

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
	virtual void OnUpdate() override;

	virtual void DoComponentInterface() override;
};