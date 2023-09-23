#pragma once

#include <vector>
#include <memory>
#include <deque>

#include <nlohmann/json.hpp>

#include "../Core/Logger.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_stdlib.h"

struct Component
{
	/**
	*Stores the entity ID for the entity this component belongs to.
	*/
	std::string m_sEntityID;

	bool m_bAvailableForReuse = false;

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
		ImGui::Text("No editable values. Component window not defined for this type.");
	}

	virtual Component* GetAsComponent(std::string entityID) = 0;
	virtual void AddComponent() = 0;
	virtual void AddComponentToEntity(std::string entityID) = 0;
	virtual void RemoveComponent(std::string entityID) = 0;
	virtual void RemoveAllComponents() = 0;

	virtual void PreUpdate() {}
	virtual void Update(bool useThreads, int maxComponentsPerThread) = 0;
	virtual void OnUpdate() {}

	virtual void PreRender() {};
	virtual void Render(bool useThreads, int maxComponentsPerThread) {};
	virtual void OnRender() {}

	virtual void LoadComponentDataFromJson(nlohmann::json& j) = 0;
	virtual nlohmann::json WriteComponentDataToJson() = 0;
};