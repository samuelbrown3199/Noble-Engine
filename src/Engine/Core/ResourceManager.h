#pragma once
#ifndef RESOURCEMANAGER_H_
#define RESOURCEMANAGER_H_

#include <vector>
#include <memory>
#include <string>

#include "../Useful.h"
#include "../Resource/Resource.h"
#include "Logger.h"

/**
*Handles resource management. Stores, loads and unloads resources.
*/
struct ResourceManager
{
	/**
	*Stores all loaded resources.
	*/
	static std::vector<std::shared_ptr<Resource>> m_vResources;
	static std::string m_sWorkingDirectory;

	ResourceManager();
	~ResourceManager();

	static std::string GetWorkingDirectory() { return m_sWorkingDirectory; }
	static void SetWorkingDirectory(std::string directory);

	template<typename T>
	static std::shared_ptr<T> PrelimLoadResource(std::string _fileDirectory)
	{
		Logger::LogInformation(FormatString("Loading asset file %s", _fileDirectory.c_str()));

		if (!PathExists(_fileDirectory))
		{
			Logger::LogError(FormatString("%s could not be found!", _fileDirectory.c_str()), 1);
			return nullptr;
		}

		for (size_t re = 0; re < m_vResources.size(); re++)
		{
			if (m_vResources.at(re)->m_sResourcePath == _fileDirectory)
			{
				std::shared_ptr<T> resource = std::dynamic_pointer_cast<T>(m_vResources.at(re));
				if (resource)
				{
					return resource;
				}
			}
		}

		return nullptr;
	}

	/**
	*Loads a resource of the passed type with the file directory.
	*/
	template<typename T>
	static std::shared_ptr<T> LoadResource(std::string _fileDirectory)
	{
		std::string searchPath = m_sWorkingDirectory + "\\" + _fileDirectory;
		
		std::shared_ptr<T> oldResource = PrelimLoadResource<T>(searchPath);
		if (oldResource != nullptr)
			return oldResource;

		std::shared_ptr<T> newResource = std::make_shared<T>();
		newResource->m_sResourcePath = searchPath;
		newResource->m_sLocalPath = _fileDirectory;
		newResource->OnLoad();
		m_vResources.push_back(newResource);
		return newResource;
	}

	/**
	*Loads a resource of the passed type with the file directory.
	*/
	template<typename T, typename ... Args>
	static std::shared_ptr<T> LoadResource(std::string _fileDirectory, Args&&... _args)
	{
		std::string searchPath = m_sWorkingDirectory + "\\" + _fileDirectory;

		std::shared_ptr<T> oldResource = PrelimLoadResource<T>(searchPath);
		if (oldResource != nullptr)
			return oldResource;

		std::shared_ptr<T> newResource = std::make_shared<T>(std::forward<Args>(_args)...);
		newResource->m_sResourcePath = searchPath;
		newResource->m_sLocalPath = _fileDirectory;
		newResource->OnLoad();
		m_vResources.push_back(newResource);
		return newResource;
	}

	/**
	*Unloads resources whose use count is currently 1. This means that un-used resources are no longer kept in memory.
	*/
	static void UnloadUnusedResources();
	static void UnloadAllResources();
};

#endif