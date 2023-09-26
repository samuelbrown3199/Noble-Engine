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

	virtual std::string GetComponentID() = 0;
	
	virtual int GetComponentIndex(std::string entityID);
	virtual Component* GetAsComponent(int index);
	virtual void AddComponent();
	virtual void AddComponentToEntity(std::string entityID);
	virtual void RemoveComponent(std::string entityID);
	virtual void RemoveAllComponents();

	template<typename T>
	T* GetComponent(std::string entityID)
	{
		return dynamic_cast<T*>(GetAsComponent(entityID));
	}

	virtual void PreUpdate() {}
	virtual void Update(bool useThreads, int maxComponentsPerThread);
	virtual void OnUpdate() {}

	virtual void PreRender() {};
	virtual void Render(bool useThreads, int maxComponentsPerThread);
	virtual void OnRender() {}

	virtual void LoadComponentDataFromJson(nlohmann::json& j);
	virtual nlohmann::json WriteComponentDataToJson();
};