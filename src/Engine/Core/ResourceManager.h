#pragma once

#include <vector>
#include <deque>
#include <memory>
#include <string>
#include <mutex>

#include <nlohmann/json.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "../imgui/imgui.h"
#include "../imgui/imgui_stdlib.h"

#include "../Useful.h"
#include "../Core/EngineResources/Resource.h"
#include "Logger.h"

/**
*Handles resource management. Stores, loads and unloads resources.
*/
struct ResourceManager
{
	FT_Library m_fontLibrary;

	std::map<std::string, std::filesystem::file_time_type> m_vPreviousScanFiles;
	/**
	*Stores all resources.
	*/
	std::map<std::string, std::shared_ptr<Resource>> m_mResourceDatabase;
	/**
	*Stores all loaded resources.
	*/
	std::map<std::string, std::shared_ptr<Resource>> m_mLoadedResources;

	std::string m_sWorkingDirectory;
	nlohmann::json m_resourceDatabaseJson;
	std::mutex m_resourceDatabaseMutex;

	std::deque<std::shared_ptr<Resource>> m_qReloadQueue;

	ResourceManager();
	~ResourceManager();

	void RegisterResourceTypes();

	std::string GetResourceManagerWorkingDirectory() { return m_sWorkingDirectory; }
	void SetWorkingDirectory(std::string directory);

	template<typename T>
	void AddNewResource(std::string path)
	{
		std::string relativeLocation = GetFolderLocationRelativeToGameData(path);
		if (m_mResourceDatabase.find(relativeLocation) != m_mResourceDatabase.end())
		{
			LogError("Tried to add a duplicate resource.");
			return;
		}

		std::shared_ptr<T> newResource = std::make_shared<T>();
		newResource->m_sResourcePath = path;
		newResource->m_sLocalPath = GetFolderLocationRelativeToGameData(path) != "" ? GetFolderLocationRelativeToGameData(path) : path;

		SetResourceToDefaults(newResource);

		m_mResourceDatabase[relativeLocation] = newResource;
		LogInfo("Added new resource " + newResource->m_sLocalPath);

		WriteResourceDatabase();
	}

	void AddNewResource(std::string type, std::string path);
	void AddNewResource(Resource* resource);

	void RemoveResourceFromDatabase(std::string localPath);
	void SetResourceToDefaults(std::shared_ptr<Resource> res);
	void LoadResourceDatabase(nlohmann::json resourceDatabase);
	nlohmann::json WriteResourceDatabase();
	
	template<typename T>
	std::shared_ptr<T> PrelimLoadResource(const std::string& _localPath, const std::map<std::string, std::shared_ptr<Resource>>& targetMap)
	{
		if (targetMap.find(_localPath) != targetMap.end())
		{
			std::shared_ptr<T> resource = std::dynamic_pointer_cast<T>(targetMap.at(_localPath));
			if (resource)
			{
				return resource;
			}
		}

		return nullptr;
	}

	std::string GetResourcePath(const std::string& _fileDirectory)
	{
		std::string gamedataDir = _fileDirectory;
		if (_fileDirectory[0] != '\\')
			gamedataDir = "\\" + _fileDirectory;
		std::string searchPath = m_sWorkingDirectory + gamedataDir;

		return searchPath;
	}

	template<typename T>
	std::shared_ptr<T> GetResourceFromDatabase(const std::string& _localPath, const bool& requiresFile)
	{
		std::shared_ptr<T> oldResource = nullptr;

		if (requiresFile)
		{
			oldResource = PrelimLoadResource<T>(_localPath, m_mResourceDatabase);
		}
		else
		{
			oldResource = PrelimLoadResource<T>(_localPath, m_mResourceDatabase);
		}

		return oldResource;
	}

	/**
	*Loads a resource of the passed type with the file directory.
	*/
	template<typename T>
	std::shared_ptr<T> LoadResource(const std::string& _fileDirectory)
	{
		std::string searchPath = GetResourcePath(_fileDirectory);
		std::shared_ptr<T> oldResource = PrelimLoadResource<T>(searchPath, m_mLoadedResources);
		if (oldResource != nullptr)
		{
			LogInfo(FormatString("Loaded asset file %s", _fileDirectory.c_str()));
			return oldResource;
		}

		oldResource = PrelimLoadResource<T>(_fileDirectory, m_mLoadedResources);
		if (oldResource != nullptr)
		{
			LogInfo(FormatString("Loaded asset file %s", _fileDirectory.c_str()));
			return oldResource;
		}

		oldResource = PrelimLoadResource<T>(searchPath, m_mResourceDatabase);
		if (oldResource != nullptr)
		{
			oldResource->OnLoad();
			m_mLoadedResources[oldResource->m_sLocalPath] = oldResource;
			LogInfo(FormatString("Loaded asset file %s", _fileDirectory.c_str()));
			return oldResource;
		}

		oldResource = PrelimLoadResource<T>(_fileDirectory, m_mResourceDatabase);
		if (oldResource != nullptr)
		{
			oldResource->OnLoad();
			m_mLoadedResources[oldResource->m_sLocalPath] = oldResource;
			LogInfo(FormatString("Loaded asset file %s", _fileDirectory.c_str()));
			return oldResource;
		}
		
		LogError(FormatString("Resource %s doesnt exist in database, make sure it is added.", _fileDirectory.c_str()));
		return nullptr;
	}

	void ScanForResources();
	void ReloadResources();
	bool IsFileInDatabase(std::string path);
	std::string GetResourceTypeFromPath(std::string path);
	std::vector<std::shared_ptr<Resource>> GetAllResourcesOfType(std::string type);
	/**
	*Unloads resources whose use count is currently 2. This means that un-used resources are no longer kept in memory.
	*/
	void UnloadUnusedResources();

	/**
	* Unloads all resources from memory.
	*/
	void UnloadAllResources();
	std::shared_ptr<Resource> DoResourceSelectInterface(std::string interfaceText, std::string currentResourcePath, std::string type);

	float GetResourceMemoryUsage();
};