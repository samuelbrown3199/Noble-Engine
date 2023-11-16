#pragma once
#ifndef BEHAVIOUR_HPP
#define BEHAVIOUR_HPP

#include <string>
#include <nlohmann/json.hpp>

#include "../../imgui/imgui.h"

struct Behaviour
{
	std::string m_sEntityID;

	virtual Behaviour* GetAsBehaviour(std::string entityID) = 0;
	virtual Behaviour* AddBehaviourToEntity(std::string entityID) = 0;
	virtual void RemoveBehaviourFromEntity(std::string entityID) = 0;

	virtual void Start() {};
	virtual void Update() {};

	virtual void DoBehaviourInterface()
	{
		ImGui::Text("No Behaviour Interface Defined.");
	}

	virtual void LoadBehaviourFromJson(std::string entityID, nlohmann::json& j) = 0;
	virtual nlohmann::json WriteBehaviourToJson() = 0;
};

#endif