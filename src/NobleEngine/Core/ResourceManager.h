#pragma once
#ifndef RESOURCEMANAGER_H_
#define RESOURCEMANAGER_H_

#include <vector>
#include <memory>
#include <string>

#include "Resource.h"

namespace NobleCore
{
	/**
	*Handles resource management. Stores, loads and unloads resources.
	*/
	struct ResourceManager
	{
		/**
		*Stores all loaded resources.
		*/
		static std::vector<std::shared_ptr<Resource>> resources;

		/**
		*Loads a resource of the passed type with the file directory.
		*/
		template<typename T>
		static std::shared_ptr<T> LoadResource(std::string _fileDirectory)
		{
			for (size_t re = 0; re < resources.size(); re++)
			{
				if (resources.at(re)->resourcePath == _fileDirectory)
				{
					std::shared_ptr<T> resource = std::dynamic_pointer_cast<T>(resources.at(re));
					if (resource)
					{
						return resource;
					}
				}
			}

			std::shared_ptr<T> newResource = std::make_shared<T>();
			newResource->resourcePath = _fileDirectory;
			newResource->OnLoad();
			resources.push_back(newResource);
			return newResource;
		}
		/**
		*Unloads resources whose use count is currently 1. This means that un-used resources are no longer kept in memory.
		*/
		static void UnloadUnusedResources();
	};
}

#endif