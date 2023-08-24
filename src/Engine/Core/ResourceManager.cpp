#include "ResourceManager.h"
#include "../Useful.h"

#include "Registry.h"

#include "../Resource/AudioClip.h"
#include "../Resource/Texture.h"
#include "../Resource/Model.h"
#include "../Resource/Script.h"

std::vector<std::shared_ptr<Resource>> ResourceManager::m_vResourceDatabase;
std::vector<std::shared_ptr<Resource>> ResourceManager::m_vLoadedResources;
std::vector<std::shared_ptr<ShaderProgram>> ResourceManager::m_vShaderPrograms;
std::string ResourceManager::m_sWorkingDirectory;

nlohmann::json ResourceManager::m_resourceDatabaseJson;

ResourceManager::ResourceManager()
{
	m_sWorkingDirectory = GetWorkingDirectory();
	RegisterResourceTypes();

	LoadResourceDatabase();
}

ResourceManager::~ResourceManager()
{
	m_vLoadedResources.clear();
}

void ResourceManager::RegisterResourceTypes()
{
	NobleRegistry::RegisterResource("AudioClip", new AudioClip());
	NobleRegistry::RegisterResource("Texture", new Texture());
	NobleRegistry::RegisterResource("Model", new Model());
	NobleRegistry::RegisterResource("Script", new Script());
	NobleRegistry::RegisterResource("Shader", new Shader());
}

void ResourceManager::SetWorkingDirectory(std::string directory)
{
	m_sWorkingDirectory = directory;
	LoadResourceDatabase();
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
	std::map<int, std::pair<std::string, Resource*>>* resourceRegistry = NobleRegistry::GetResourceRegistry();

	bool foundRegistryRes = false;
	for (int i = 0; i < resourceRegistry->size(); i++)
	{
		if (resourceRegistry->at(i).first == res->m_resourceType)
		{
			resourceRegistry->at(i).second->SetResourceToDefaults(res);
			foundRegistryRes = true;
			break;
		}
	}

	if (!foundRegistryRes)
		Logger::LogError("Couldnt find resource type in Registry. Has it been registered?", 2);
}

void ResourceManager::LoadResourceDatabase()
{
	std::string databasePath = m_sWorkingDirectory + "\\ResourceDatabase.nrd";
	if (!PathExists(databasePath))
		return;

	m_vResourceDatabase.clear();

	std::ifstream database(databasePath);
	if (database.is_open())
	{
		m_resourceDatabaseJson = nlohmann::json::parse(database);
		database.close();
	}

	std::map<int, std::pair<std::string, Resource*>>* resourceRegistry = NobleRegistry::GetResourceRegistry();

	if (m_resourceDatabaseJson.find("Defaults") != m_resourceDatabaseJson.end())
	{
		nlohmann::json def = m_resourceDatabaseJson.at("Defaults");

		for (int i = 0; i < resourceRegistry->size(); i++)
		{
			if (def.find(resourceRegistry->at(i).first) != def.end())
			{
				resourceRegistry->at(i).second->SetDefaults(def[resourceRegistry->at(i).first]);
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
				std::shared_ptr<Resource> res = resourceRegistry->at(i).second->LoadFromJson(it.key(), it.value());
				m_vResourceDatabase.push_back(res);
			}

			Logger::LogInformation(FormatString("Loaded %d %s", ac.size(), resourceRegistry->at(i).first.c_str()));
		}
	}

	if (m_resourceDatabaseJson.find("ShaderPrograms") != m_resourceDatabaseJson.end())
	{
		nlohmann::json ac = m_resourceDatabaseJson.at("ShaderPrograms");
		for (auto it : ac.items())
		{
			std::shared_ptr<ShaderProgram> newProgram = std::make_shared<ShaderProgram>();
			newProgram->m_shaderProgramID = it.key();
			newProgram->LoadFromJson(it.value());

			newProgram->LinkShaderProgram(newProgram);

			m_vShaderPrograms.push_back(newProgram);
		}

		Logger::LogInformation(FormatString("Loaded %d Shader Programs", ac.size()));
	}
}

void ResourceManager::WriteResourceDatabase()
{
	m_resourceDatabaseJson.clear();
	for (size_t re = 0; re < m_vResourceDatabase.size(); re++)
	{
		m_resourceDatabaseJson[m_vResourceDatabase.at(re)->m_resourceType][m_vResourceDatabase.at(re)->m_sLocalPath] = m_vResourceDatabase.at(re)->AddToDatabase();
	}

	std::map<int, std::pair<std::string, Resource*>>* resourceRegistry = NobleRegistry::GetResourceRegistry();
	for (int i = 0; i < resourceRegistry->size(); i++)
	{
		m_resourceDatabaseJson["Defaults"][resourceRegistry->at(i).first] = resourceRegistry->at(i).second->AddToDatabase();
	}

	for (int i = 0; i < m_vShaderPrograms.size(); i++)
	{
		m_resourceDatabaseJson["ShaderPrograms"][m_vShaderPrograms.at(i)->m_shaderProgramID] = m_vShaderPrograms.at(i)->WriteToJson();
	}

	std::fstream database(m_sWorkingDirectory + "\\ResourceDatabase.nrd", 'w');
	database << m_resourceDatabaseJson.dump();
	database.close();

	Logger::LogInformation("Resource Database file has been updated.");
}

std::shared_ptr<ShaderProgram> ResourceManager::CreateShaderProgram(std::string shaderProgramID)
{
	if (shaderProgramID.empty())
		return nullptr;

	std::shared_ptr<ShaderProgram> rtn = std::make_shared<ShaderProgram>();
	rtn->m_shaderProgramID = shaderProgramID;
	m_vShaderPrograms.push_back(rtn);
	WriteResourceDatabase();

	Logger::LogInformation("Created new shader program.");

	return rtn;
}

std::shared_ptr<ShaderProgram> ResourceManager::GetShaderProgram(std::string shaderProgramID)
{
	for (int i = 0; i < m_vShaderPrograms.size(); i++)
	{
		if (m_vShaderPrograms.at(i)->m_shaderProgramID == shaderProgramID)
			return m_vShaderPrograms.at(i);
	}

	return nullptr;
}

std::vector<std::shared_ptr<ShaderProgram>>* ResourceManager::GetShaderPrograms()
{
	return &m_vShaderPrograms;
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
	m_vShaderPrograms.clear();
}