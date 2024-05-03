#pragma once
#ifndef RESOURCE_H
#define RESOURCE_H

#include <string>
#include <exception>

#include <nlohmann/json.hpp>

#include "Engine/Core/Logger.h"

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
	virtual void OnLoad()
	{
		if (!CheckResourceFileExists())
			LogFatalError(FormatString("Resource Path does not exist: %s", m_sResourcePath));
	};
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
	

	bool m_bInitializeInterface = true;
	void InitializeInterface()
	{
		m_bInitializeInterface = true;
	}
	virtual void DoResourceInterface()
	{
		ImGui::Text(m_sLocalPath.c_str());
		ImGui::Text(m_resourceType.c_str());
		ImGui::Text("No editable values. Resource window not defined for this type.");

		m_bInitializeInterface = false;
	}

	virtual nlohmann::json AddToDatabase()
	{
		nlohmann::json data;
		return data;
	};
	virtual std::shared_ptr<Resource> LoadFromJson(const std::string& path, const nlohmann::json& data) = 0;
	virtual void SetDefaults(const nlohmann::json& data) {};

	bool CheckResourceFileExists()
	{
		return PathExists(m_sResourcePath);
	}
};
#endif