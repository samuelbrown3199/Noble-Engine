#pragma once
#ifndef RESOURCE_H
#define RESOURCE_H

#include <string>
#include <exception>

#include <nlohmann/json.hpp>

#include "../../Useful.h"
#include "../../imgui/imgui.h"

/**
*A struct that other types of resources inherit from, e.g textures and models.
*/
struct Resource
{
public:

	bool m_bIsLoaded = false;

	std::string m_resourceType;
	std::string m_sLocalPath;
	/**
	*Stores the path of the resource.
	*/
	std::string m_sResourcePath;

	/**
	*Loads the resource.
	*/
	virtual void OnLoad() = 0;
	/**
	*Unloads the resource.
	*/
	virtual void OnUnload() = 0;

	virtual void AddResource(std::string path) = 0;
	virtual std::vector<std::shared_ptr<Resource>> GetResourcesOfType() = 0;

	virtual void SetResourceToDefaults(std::shared_ptr<Resource> res) {};

	void ReloadResource()
	{
		if (!m_bIsLoaded)
			return;

		OnUnload();
		OnLoad();
	}

	/**
	*Saves the resource.
	*/
	virtual void OnSave() {};

	virtual void DoResourceInterface()
	{
		ImGui::Text(m_sLocalPath.c_str());
		ImGui::Text(m_resourceType.c_str());
		ImGui::Text("No editable values. Resource window not defined for this type.");
	}

	virtual nlohmann::json AddToDatabase()
	{
		nlohmann::json data;
		return data;
	};
	virtual std::shared_ptr<Resource> LoadFromJson(const std::string& path, const nlohmann::json& data) = 0;
	virtual void SetDefaults(const nlohmann::json& data) {};
};
#endif