#pragma once
#ifndef RESOURCEMANAGER_H_
#define RESOURCEMANAGER_H_

#include <vector>
#include <memory>
#include <string>

#include "../Useful.h"
#include "../Resource/Resource.h"
#include "../Resource/ShaderProgram.hpp"
#include "Logger.h"

#include "../Resource/Font.h"

#include <ft2build.h>
#include FT_FREETYPE_H

/**
*Handles resource management. Stores, loads and unloads resources.
*/
struct ResourceManager
{
	/**
	*Stores all loaded resources.
	*/
	static std::vector<std::shared_ptr<Resource>> m_vResources;
	static std::vector<std::shared_ptr<ShaderProgram>> m_vShaderPrograms;
	static std::string m_sWorkingDirectory;

	static FT_Library m_fontLibrary;

	ResourceManager();

	/**
	*Loads a resource of the passed type with the file directory.
	*/
	template<typename T>
	static std::shared_ptr<T> LoadResource(std::string _fileDirectory)
	{
		std::string searchPath = m_sWorkingDirectory + "\\" + _fileDirectory;
		Logger::LogInformation(FormatString("Loading asset file %s", searchPath.c_str()));

		if (!PathExists(searchPath))
		{
			Logger::LogError(FormatString("%s could not be found!", searchPath.c_str()), 1);
			return nullptr;
		}

		for (size_t re = 0; re < m_vResources.size(); re++)
		{
			if (m_vResources.at(re)->m_sResourcePath == searchPath)
			{
				std::shared_ptr<T> resource = std::dynamic_pointer_cast<T>(m_vResources.at(re));
				if (resource)
				{
					return resource;
				}
			}
		}

		std::shared_ptr<T> newResource = std::make_shared<T>();
		newResource->m_sResourcePath = searchPath;
		newResource->OnLoad();
		m_vResources.push_back(newResource);
		return newResource;
	}
	/**
	* Creates a shader program and stores it in the shader program list.
	*/
	static std::shared_ptr<ShaderProgram> CreateShaderProgram()
	{
		std::shared_ptr<ShaderProgram> rtn = std::make_shared<ShaderProgram>();
		m_vShaderPrograms.push_back(rtn);
		Logger::LogInformation("Created new shader program.");

		return rtn;
	}
	static std::shared_ptr<Font> LoadFont(std::string fontPath, int fontPixelSize);
	/**
	*Unloads resources whose use count is currently 1. This means that un-used resources are no longer kept in memory.
	*/
	static void UnloadUnusedResources();
};

#endif