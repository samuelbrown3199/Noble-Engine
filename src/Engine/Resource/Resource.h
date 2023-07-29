#pragma once
#ifndef RESOURCE_H
#define RESOURCE_H

#include <string>
#include <exception>

#include <nlohmann/json.hpp>

#include "../Useful.h"

/**
*A struct that other types of resources inherit from, e.g textures and models.
*/
struct Resource
{
public:

	bool m_bIsLoaded;

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

	/**
	*Saves the resource.
	*/
	virtual void OnSave() {};

	virtual nlohmann::json AddToDatabase()
	{
		nlohmann::json data;
		return data;
	};
	virtual void LoadFromJson(std::string path, nlohmann::json data)
	{
		m_sLocalPath = path;
		m_sResourcePath = GetGameFolder() + path;
	}
};
#endif