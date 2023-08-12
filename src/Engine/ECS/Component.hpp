#pragma once

#include <vector>
#include <memory>

#include <nlohmann/json.hpp>

#include "System.hpp"
#include "../imgui/imgui.h"

struct Component
{
	/**
	*Stores the entity ID for the entity this component belongs to.
	*/
	std::string m_sEntityID;

	/**
	* This can be used to create initialisation functions for components.
	*/
	virtual void OnInitialize() {};
	/**
	* This is used when deleting component data. Useful for cleaning up memory etc.
	*/
	virtual void OnRemove() {};

	virtual nlohmann::json WriteJson() { nlohmann::json data; return data; };
	virtual void FromJson(const nlohmann::json& j) {};

	virtual void DoComponentInterface()
	{
		ImGui::Text(m_sEntityID.c_str());
		ImGui::Text("No editable values. Component window not defined for this type.");
	}
};