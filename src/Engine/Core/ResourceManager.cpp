#include "ResourceManager.h"
#include "../Useful.h"

#include "Registry.h"

#include "../Core/EngineResources/AudioClip.h"
#include "../Core/EngineResources/Texture.h"
#include "../Core/EngineResources/Model.h"
#include "../Core/EngineResources/Pipeline.h"
#include "../Core/EngineResources/Script.h"
#include "ProjectFile.h"

ResourceManager::ResourceManager()
{
	if (FT_Init_FreeType(&m_fontLibrary))
	{
		Logger::LogError("Could not initialize FreeType Library!", 2);
	}

	m_sWorkingDirectory = GetWorkingDirectory();
	RegisterResourceTypes();
}

ResourceManager::~ResourceManager()
{
	m_vLoadedResources.clear();
}

void ResourceManager::RegisterResourceTypes()
{
	NobleRegistry::RegisterResource("AudioClip", new AudioClip(), true);
	NobleRegistry::RegisterResource("Texture", new Texture(), true);
	NobleRegistry::RegisterResource("Model", new Model(), true);
	NobleRegistry::RegisterResource("Pipeline", new Pipeline(), false);
	NobleRegistry::RegisterResource("Script", new Script(), true);
	NobleRegistry::RegisterResource("Shader", new Shader(), true);
}

void ResourceManager::SetWorkingDirectory(std::string directory)
{
	m_sWorkingDirectory = directory;
}

void ResourceManager::RemoveResourceFromDatabase(std::string path)
{
	for (int i = 0; i < m_vResourceDatabase.size(); i++)
	{
		if (m_vResourceDatabase.at(i)->m_sLocalPath == path)
		{
			m_vResourceDatabase.erase(m_vResourceDatabase.begin() + i);
			WriteResourceDatabase();
			Logger::LogInformation(FormatString("Remove asset %s from Resource Database", path.c_str()));
			return;
		}
	}
}

void ResourceManager::SetResourceToDefaults(std::shared_ptr<Resource> res)
{
	std::map<int, std::pair<std::string, ResourceRegistry>>* resourceRegistry = NobleRegistry::GetResourceRegistry();

	bool foundRegistryRes = false;
	for (int i = 0; i < resourceRegistry->size(); i++)
	{
		if (resourceRegistry->at(i).first == res->m_resourceType)
		{
			resourceRegistry->at(i).second.m_resource->SetResourceToDefaults(res);
			foundRegistryRes = true;
			break;
		}
	}

	if (!foundRegistryRes)
		Logger::LogError("Couldnt find resource type in Registry. Has it been registered?", 2);
}

void ResourceManager::LoadResourceDatabase(nlohmann::json resourceDatabase)
{
	m_vResourceDatabase.clear();
	m_resourceDatabaseJson = resourceDatabase;

	std::map<int, std::pair<std::string, ResourceRegistry>>* resourceRegistry = NobleRegistry::GetResourceRegistry();

	if (m_resourceDatabaseJson.find("Defaults") != m_resourceDatabaseJson.end())
	{
		nlohmann::json def = m_resourceDatabaseJson.at("Defaults");

		for (int i = 0; i < resourceRegistry->size(); i++)
		{
			if (def.find(resourceRegistry->at(i).first) != def.end())
			{
				resourceRegistry->at(i).second.m_resource->SetDefaults(def[resourceRegistry->at(i).first]);
			}
		}
	}

	for (int i = 0; i < resourceRegistry->size(); i++)
	{
		if (m_resourceDatabaseJson.find(resourceRegistry->at(i).first) != m_resourceDatabaseJson.end())
		{
			nlohmann::json ac = m_resourceDatabaseJson.at(resourceRegistry->at(i).first);
			for (auto it : ac.items())
			{
				std::shared_ptr<Resource> res = resourceRegistry->at(i).second.m_resource->LoadFromJson(it.key(), it.value());
				m_vResourceDatabase.push_back(res);
			}

			Logger::LogInformation(FormatString("Loaded %d %s", ac.size(), resourceRegistry->at(i).first.c_str()));
		}
	}
}

nlohmann::json ResourceManager::WriteResourceDatabase()
{
	m_resourceDatabaseJson.clear();

	for (size_t re = 0; re < m_vResourceDatabase.size(); re++)
	{
		m_resourceDatabaseJson[m_vResourceDatabase.at(re)->m_resourceType][m_vResourceDatabase.at(re)->m_sLocalPath] = m_vResourceDatabase.at(re)->AddToDatabase();
	}

	std::map<int, std::pair<std::string, ResourceRegistry>>* resourceRegistry = NobleRegistry::GetResourceRegistry();
	for (int i = 0; i < resourceRegistry->size(); i++)
	{
		m_resourceDatabaseJson["Defaults"][resourceRegistry->at(i).first] = resourceRegistry->at(i).second.m_resource->AddToDatabase();
	}
	return m_resourceDatabaseJson;
}

void ResourceManager::UnloadUnusedResources()
{
	if (m_vLoadedResources.empty())
		return;

	for (size_t re = m_vLoadedResources.size()-1; re > 0 ; re--)
	{
		if (m_vLoadedResources.at(re).use_count() == 2)
		{
			Logger::LogInformation("Unloaded " + m_vLoadedResources.at(re)->m_sLocalPath);

			m_vLoadedResources.at(re)->OnUnload();
			m_vLoadedResources.erase(m_vLoadedResources.begin() + re);
		}
	}
}

void ResourceManager::UnloadAllResources()
{
	m_vLoadedResources.clear();
	m_vResourceDatabase.clear();
}