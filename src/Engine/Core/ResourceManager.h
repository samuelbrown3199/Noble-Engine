#pragma once
#ifndef RESOURCEMANAGER_H_
#define RESOURCEMANAGER_H_

#include <vector>
#include <memory>
#include <string>

#include <nlohmann/json.hpp>

#include "../imgui/imgui.h"
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

	void RegisterResourceTypes();

	static std::string GetResourceManagerWorkingDirectory() { return m_sWorkingDirectory; }
	static void SetWorkingDirectory(std::string directory);

	template<typename T>
	static void AddNewResource(std::string path)
	{
		for (int i = 0; i < m_vResourceDatabase.size(); i++)
		{
			if (m_vResourceDatabase.at(i)->m_sResourcePath == path)
			{
				Logger::LogError("Tried to add a duplicate resource.", 0);
				return;
			}
		}

		std::shared_ptr<T> newResource = std::make_shared<T>();
		newResource->m_sResourcePath = path;
		newResource->m_sLocalPath = GetFolderLocationRelativeToGameData(path);

		SetResourceToDefaults(newResource);

		m_vResourceDatabase.push_back(newResource);
		Logger::LogInformation("Added new resource " + newResource->m_sLocalPath);

		WriteResourceDatabase();
	}

	static void RemoveResourceFromDatabase(std::string path);

	static void SetResourceToDefaults(std::shared_ptr<Resource> res);
	
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

	template<typename T>
	static std::vector<std::shared_ptr<Resource>> GetAllResourcesOfType()
	{
		std::vector<std::shared_ptr<Resource>> returnVec;

		for (int i = 0; i < m_vResourceDatabase.size(); i++)
		{
			std::shared_ptr<T> resource = std::dynamic_pointer_cast<T>(m_vResourceDatabase.at(i));
			if (resource != nullptr)
				returnVec.push_back(m_vResourceDatabase.at(i));
		}

		return returnVec;
	}
	
	/**
	*Unloads resources whose use count is currently 1. This means that un-used resources are no longer kept in memory.
	*/
	static void UnloadUnusedResources();
	static void UnloadAllResources();



	template<typename T>
	static std::shared_ptr<T> DoResourceSelectInterface(std::string interfaceText, std::string currentResourcePath)
	{
		std::vector<std::shared_ptr<Resource>> resources = GetAllResourcesOfType<T>();

		if (resources.size() == 0)
		{
			ImGui::Text(FormatString("No resources of type %s exist in database.", interfaceText).c_str());
			return nullptr;
		}

		int res = 0;
		for (int i = 0; i < resources.size(); i++)
		{
			if (resources.at(i)->m_sLocalPath == currentResourcePath)
				res = i;
		}

		ImGui::Text(interfaceText.c_str());

		for (int i = 0; i < resources.size(); i++)
		{
			if (ImGui::Selectable(resources.at(i)->m_sLocalPath.c_str(), res == i))
				res = i;
		}

		ImGui::Dummy(ImVec2(0.0f, 5.0f));

		if (resources.at(res)->m_sLocalPath == currentResourcePath)
		{
			return std::dynamic_pointer_cast<T>(resources.at(res));
		}

		return LoadResource<T>(resources.at(res)->m_sLocalPath);
	}
};

#endif