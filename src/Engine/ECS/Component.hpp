#pragma once

#include <vector>
#include <memory>

#include <nlohmann/json.hpp>

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
	virtual void FromJson(const nlohmann::json& j) {}

	virtual void DoComponentInterface()
	{
		ImGui::Text(m_sEntityID.c_str());
		ImGui::Text("No editable values. Component window not defined for this type.");
	}

	virtual Component* GetAsComponent(std::string entityID) = 0;
	virtual void AddComponent() = 0;
	virtual void RemoveComponent(std::string entityID) = 0;
	virtual void RemoveAllComponents() = 0;

	virtual void PreUpdate() {}
	virtual void Update(bool useThreads, int maxComponentsPerThread) = 0;
	virtual void ThreadUpdate(int _buffer, int _amount) = 0;
	virtual void OnUpdate() {}

	virtual void PreRender() {};
	virtual void Render(bool useThreads, int maxComponentsPerThread) = 0;
	virtual void ThreadRender(int _buffer, int _amount) = 0;
	virtual void OnRender() {}

	virtual void LoadComponentDataFromJson(nlohmann::json& j) = 0;
	virtual nlohmann::json WriteComponentDataToJson() = 0;
};