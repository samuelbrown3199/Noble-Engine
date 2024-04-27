#pragma once

#include <FMOD/fmod.h>

#include "../ECS/Component.h"
#include "Transform.h"

struct AudioListener : Component
{
	int m_transformIndex = -1;
	glm::vec3 m_velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 m_up = glm::vec3(0.0f, 1.0f, 0.0f);

	static int m_iCurrentListener;
	static FMOD_VECTOR pos;
	static FMOD_VECTOR vel;
	static FMOD_VECTOR forward;
	static FMOD_VECTOR up;

	void OnRemove() override
	{
		m_transformIndex = -1;
	}

	std::string GetComponentID() override
	{
		return "AudioListener";
	}

	nlohmann::json WriteJson() override
	{
		nlohmann::json data = { {"velocity", {m_velocity.x, m_velocity.y, m_velocity.z }} };
		return data;
	}

	void FromJson(const nlohmann::json& j) override
	{
		m_velocity = glm::vec3(j["velocity"][0], j["velocity"][1], j["velocity"][2]);
	}

	virtual void DoComponentInterface() override;

	virtual void PreUpdate() override;
	virtual void OnUpdate() override;
};