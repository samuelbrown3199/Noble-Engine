#pragma once

#include <FMOD/fmod.h>

#include "../ECS/ComponentList.hpp"
#include "../ECS/Component.hpp"
#include "Transform.h"

struct AudioListener : Component
{
	Transform* m_listenerTransform = nullptr;
	glm::vec3 m_velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 m_up = glm::vec3(0.0f, 1.0f, 0.0f);

	static int m_iCurrentListener;
	static FMOD_VECTOR pos;
	static FMOD_VECTOR vel;
	static FMOD_VECTOR forward;
	static FMOD_VECTOR up;

	static ComponentDatalist<AudioListener> m_componentList;

	nlohmann::json WriteJson() override
	{
		nlohmann::json data = { {"velocity", {m_velocity.x, m_velocity.y, m_velocity.z }} };
		return data;
	}

	void FromJson(const nlohmann::json& j) override
	{
		m_velocity = glm::vec3(j["velocity"][0], j["velocity"][1], j["velocity"][2]);
	}

	Component* GetAsComponent(std::string entityID) override
	{
		return GetComponent(entityID);
	}

	virtual void DoComponentInterface() override
	{
		float vel[3] = { m_velocity.x,m_velocity.y ,m_velocity.z };
		ImGui::DragFloat3("Velocity", vel);
		m_velocity = glm::vec3(vel[0], vel[1], vel[2]);
	}

	virtual void AddComponent() override;
	virtual void AddComponentToEntity(std::string entityID) override;
	virtual void RemoveComponent(std::string entityID) override;
	virtual void RemoveAllComponents() override;

	AudioListener* GetComponent(std::string entityID);

	virtual void PreUpdate() override;
	virtual void Update(bool useThreads, int maxComponentsPerThread) override;
	virtual void OnUpdate() override;

	virtual void Render(bool useThreads, int maxComponentsPerThread) override;

	virtual void LoadComponentDataFromJson(nlohmann::json& j) override;
	virtual nlohmann::json WriteComponentDataToJson() override;
};