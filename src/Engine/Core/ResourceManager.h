#pragma once
#ifndef RESOURCEMANAGER_H_
#define RESOURCEMANAGER_H_

#include <vector>
#include <memory>
#include <string>

#include <nlohmann/json.hpp>

#include "../Useful.h"
#include "../Resource/Resource.h"
#include "Logger.h"

/**
*Handles resource management. Stores, loads and unloads resources.
*/
struct ResourceManager
{
	/**
	*Stores all resources.
	*/
	static std::vector<std::shared_ptr<Resource>> m_vResourceDatabase;
	/**
	*Stores all loaded resources.
	*/
	static std::vector<std::shared_ptr<Resource>> m_vLoadedResources;
	static std::string m_sWorkingDirectory;

	static nlohmann::json m_resourceDatabaseJson;

	ResourceManager();
	~ResourceManager();

	static std::string GetWorkingDirectory() { return m_sWorkingDirectory; }
	static void SetWorkingDirectory(std::string directory);

	template<typename T>
	static void AddNewResource(std::string path)
	{
		std::shared_ptr<T> newResource = std::make_shared<T>();
		newResource->m_sResourcePath = path;
		newResource->m_sLocalPath = GetFolderLocationRelativeToGameData(path);

		m_vResourceDatabase.push_back(newResource);
		Logger::LogInformation("Added new resource " + newResource->m_sLocalPath);

		WriteResourceDatabase();
	}
	
	static void LoadResourceDatabase();
	static void WriteResourceDatabase();
	
	template<typename T>
	static std::shared_ptr<T> PrelimLoadResource(const std::string& _fileDirectory, const std::vector<std::shared_ptr<Resource>>& targetVector)
	{
		if (!PathExists(_fileDirectory))
		{
			Logger::LogError(FormatString("%s could not be found!", _fileDirectory.c_str()), 2);
		}

		for (size_t re = 0; re < targetVector.size(); re++)
		{
			if (targetVector.at(re)->m_sResourcePath == _fileDirectory)
			{
				std::shared_ptr<T> resource = std::dynamic_pointer_cast<T>(targetVector.at(re));
				if (resource)
				{
					return resource;
				}
			}
		}

		return nullptr;
	}

	static std::string GetResourcePath(const std::string& _fileDirectory)
	{
		std::string gamedataDir = _fileDirectory;
		if (_fileDirectory[0] != '\\')
			gamedataDir = "\\" + _fileDirectory;
		std::string searchPath = m_sWorkingDirectory + gamedataDir;

		return searchPath;
	}

	template<typename T>
	static std::shared_ptr<T> GetResourceFromDatabase(const std::string& _fileDirectory)
	{
		std::shared_ptr<T> oldResource = PrelimLoadResource<T>(GetResourcePath(_fileDirectory), m_vResourceDatabase);
		return oldResource;
	}

	/**
	*Loads a resource of the passed type with the file directory.
	*/
	template<typename T>
	static std::shared_ptr<T> LoadResource(const std::string& _fileDirectory)
	{
		std::string searchPath = GetResourcePath(_fileDirectory);
		std::shared_ptr<T> oldResource = PrelimLoadResource<T>(searchPath, m_vLoadedResources);
		if (oldResource != nullptr)
		{
			Logger::LogInformation(FormatString("Loaded asset file %s", _fileDirectory.c_str()));
			return oldResource;
		}

		oldResource = PrelimLoadResource<T>(searchPath, m_vResourceDatabase);
		if (oldResource != nullptr)
		{
			oldResource->OnLoad();
			m_vLoadedResources.push_back(oldResource);
			Logger::LogInformation(FormatString("Loaded asset file %s", _fileDirectory.c_str()));
			return oldResource;
		}
		
		Logger::LogError(FormatString("Resource %s doesnt exist in database, make sure it is added.", _fileDirectory.c_str()), 1);
		return nullptr;
	}

	/**
	*Unloads resources whose use count is currently 1. This means that un-used resources are no longer kept in memory.
	*/
	static void UnloadUnusedResources();
	static void UnloadAllResources();
};

#endif