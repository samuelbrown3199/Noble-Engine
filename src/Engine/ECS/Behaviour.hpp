#pragma once
#ifndef BEHAVIOUR_HPP
#define BEHAVIOUR_HPP

#include <string>

#include "../imgui/imgui.h"

struct Behaviour
{
	std::string m_sEntityID;

	virtual Behaviour* GetAsBehaviour(std::string entityID) = 0;
	virtual void AddBehaviourToEntity(std::string entityID) = 0;
	virtual void RemoveBehaviourFromEntity(std::string entityID) = 0;

	virtual void Start() {};
	virtual void Update() {};

	virtual void DoBehaviourInterface()
	{
		ImGui::Text("No Behaviour Interface Defined.");
	}
};

#endif