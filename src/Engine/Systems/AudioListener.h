#pragma once

#include "../ECS/Component.hpp"
#include "Transform.h"

struct AudioListener : Component
{
	Transform* m_listenerTransform = nullptr;
	glm::vec3 m_velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 m_up = glm::vec3(0.0f, 1.0f, 0.0f);

	nlohmann::json WriteJson() override
	{
		nlohmann::json data = { {"velocity", {m_velocity.x, m_velocity.y, m_velocity.z }} };
		return data;
	}

	void FromJson(const nlohmann::json& j) override
	{
		m_velocity = glm::vec3(j["velocity"][0], j["velocity"][1], j["velocity"][2]);
	}
};