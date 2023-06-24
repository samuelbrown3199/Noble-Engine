#pragma once
#ifndef TRANSFORM_H_
#define TRANSFORM_H_

#include "../ECS/Component.hpp"

#include <glm/glm.hpp>

struct Transform : ComponentData<Transform>
{
	glm::vec3 m_prevPosition;
	glm::vec3 m_prevRotation;
	glm::vec3 m_prevScale;

	glm::vec3 m_position;
	glm::vec3 m_rotation;
	glm::vec3 m_scale;

	glm::mat4 m_transformMat;

	nlohmann::json WriteJson() override
	{
		nlohmann::json data = { {"position",{m_position.x, m_position.y, m_position.z}}, {"rotation",{m_rotation.x, m_rotation.y, m_rotation.z}}, {"scale",{m_scale.x, m_scale.y, m_scale.z}} };
		return data;
	}

	void FromJson(const nlohmann::json& j) override
	{
		m_position = glm::vec3(j.at("position")[0], j.at("position")[1], j.at("position")[2]);
		m_rotation = glm::vec3(j.at("rotation")[0], j.at("rotation")[1], j.at("rotation")[2]);
		m_scale= glm::vec3(j.at("scale")[0], j.at("scale")[1], j.at("scale")[2]);
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
};

#endif