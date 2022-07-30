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