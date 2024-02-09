#pragma once
#ifndef DEBUGCAMSYSTEM_H_
#define DEBUGCAMSYSTEM_H_

#include "../ECS/Behaviour.hpp"

#include <glm/glm.hpp>

struct DebugCam : public Behaviour
{
	float m_fMovementSpeed = 16.0f, sensitivity = 0.1f;
	glm::vec2 newMousePos = glm::vec2(), oldMousePos = glm::vec2();
	bool firstMouse = false;
	float pitch = 0.0f, yaw = -90.0f;

	Behaviour* GetAsBehaviour(std::string entityID) override;
	Behaviour* AddBehaviourToEntity(std::string entityID) override;
	void RemoveBehaviourFromEntity(std::string entityID) override;

	void Start();
	void Update();
	void UpdateControls();
	void UpdateCameraRotation();

	virtual void DoBehaviourInterface() override
	{
		ImGui::SliderFloat("Move Speed", &m_fMovementSpeed, 5, 64);
		ImGui::SliderFloat("Sensitivity", &sensitivity, 0.1f, 1.0f);
	}

	virtual void LoadBehaviourFromJson(std::string entityID, nlohmann::json& j) override;
	virtual nlohmann::json WriteBehaviourToJson() override;
};

#endif