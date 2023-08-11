#include "ResourceManager.h"
#include "../Useful.h"

#include "Registry.h"

#include "../Resource/AudioClip.h"
#include "../Resource/Texture.h"
#include "../Resource/Model.h"
#include "../Resource/Script.h"

std::vector<std::shared_ptr<Resource>> ResourceManager::m_vResourceDatabase;
std::vector<std::shared_ptr<Resource>> ResourceManager::m_vLoadedResources;
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
}

void ResourceManager::SetWorkingDirectory(std::string directory)
{
	m_sWorkingDirectory = directory;
	LoadResourceDatabase();
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

	//Need to have a think on this at some point.
	if (m_resourceDatabaseJson.find("AudioClip") != m_resourceDatabaseJson.end())
	{
		nlohmann::json ac = m_resourceDatabaseJson.at("AudioClip");

		for (auto it : ac.items())
		{
			std::shared_ptr<AudioClip> clip = std::make_shared<AudioClip>();
			clip->LoadFromJson(it.key(), it.value());

			m_vResourceDatabase.push_back(clip);
		}

		Logger::LogInformation(FormatString("Loaded %d AudioClips", ac.size()));
	}

	if (m_resourceDatabaseJson.find("Texture") != m_resourceDatabaseJson.end())
	{
		nlohmann::json tex = m_resourceDatabaseJson.at("Texture");

		for (auto it : tex.items())
		{
			std::shared_ptr<Texture> clip = std::make_shared<Texture>();
			clip->LoadFromJson(it.key(), it.value());

			m_vResourceDatabase.push_back(clip);
		}

		Logger::LogInformation(FormatString("Loaded %d Textures", tex.size()));
	}

	if (m_resourceDatabaseJson.find("Model") != m_resourceDatabaseJson.end())
	{
		nlohmann::json mod = m_resourceDatabaseJson.at("Model");

		for (auto it : mod.items())
		{
			std::shared_ptr<Model> clip = std::make_shared<Model>();
			clip->LoadFromJson(it.key(), it.value());

			m_vResourceDatabase.push_back(clip);
		}

		Logger::LogInformation(FormatString("Loaded %d Models", mod.size()));
	}

	if (m_resourceDatabaseJson.find("Script") != m_resourceDatabaseJson.end())
	{
		nlohmann::json scr = m_resourceDatabaseJson.at("Script");

		for (auto it : scr.items())
		{
			std::shared_ptr<Script> clip = std::make_shared<Script>();
			clip->LoadFromJson(it.key(), it.value());

			m_vResourceDatabase.push_back(clip);
		}

		Logger::LogInformation(FormatString("Loaded %d Scripts", scr.size()));
	}
}

void ResourceManager::WriteResourceDatabase()
{
	m_resourceDatabaseJson.clear();
	for (size_t re = 0; re < m_vResourceDatabase.size(); re++)
	{
		m_resourceDatabaseJson[m_vResourceDatabase.at(re)->m_resourceType][m_vResourceDatabase.at(re)->m_sLocalPath] = m_vResourceDatabase.at(re)->AddToDatabase();
	}

	std::fstream database(m_sWorkingDirectory + "\\ResourceDatabase.nrd", 'w');
	database << m_resourceDatabaseJson.dump();
	database.close();

	Logger::LogInformation("Resource Database file has been updated.");
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