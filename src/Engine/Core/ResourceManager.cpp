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
		LogFatalError("Could not initialize FreeType Library!");
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
	NobleRegistry* registry = Application::GetApplication()->GetRegistry();

	registry->RegisterResource<AudioClip>("AudioClip", true, ".wav|.ogg|.mp3");
	registry->RegisterResource<Texture>("Texture", true, ".jpg|.png|.tga|.bmp|.hdr");
	registry->RegisterResource<Model>("Model", true, ".obj");
	registry->RegisterResource<Pipeline>("Pipeline", false, ".npl");
	registry->RegisterResource<Script>("Script", true, ".lua");
	registry->RegisterResource<Shader>("Shader", true, ".vert|.frag|.comp|.spv");
}

void ResourceManager::SetWorkingDirectory(std::string directory)
{
	m_sWorkingDirectory = directory;
}

void ResourceManager::AddNewResource(std::string type, std::string path)
{
	NobleRegistry* registry = Application::GetApplication()->GetRegistry();
	std::vector<std::pair<std::string, ResourceRegistryBase*>>* resourceRegistry = registry->GetResourceRegistry();

	for (int i = 0; i < resourceRegistry->size(); i++)
	{
		if (resourceRegistry->at(i).first == type)
		{
			resourceRegistry->at(i).second->AddResourceToDatabase(path);
			return;
		}
	}

	LogFatalError("Could not find resource type in registry.");
}

void ResourceManager::AddNewResource(Resource* resource)
{
	std::shared_ptr<Resource> res(resource);

	m_vResourceDatabase.push_back(res);
	LogInfo(FormatString("Added new resource %s", res->m_sLocalPath.c_str()));

	Application::GetApplication()->GetProjectFile()->UpdateProjectFile();
}

void ResourceManager::RemoveResourceFromDatabase(std::string path)
{
	for (int i = 0; i < m_vResourceDatabase.size(); i++)
	{
		if (m_vResourceDatabase.at(i)->m_sLocalPath == path)
		{
			m_vResourceDatabase.erase(m_vResourceDatabase.begin() + i);
			LogInfo(FormatString("Remove asset %s from Resource Database", path.c_str()));
			Application::GetApplication()->GetProjectFile()->UpdateProjectFile();
			return;
		}
	}
}

void ResourceManager::SetResourceToDefaults(std::shared_ptr<Resource> res)
{
	NobleRegistry* registry = Application::GetApplication()->GetRegistry();
	std::vector<std::pair<std::string, ResourceRegistryBase*>>* resourceRegistry = registry->GetResourceRegistry();

	bool foundRegistryRes = false;
	for (int i = 0; i < resourceRegistry->size(); i++)
	{
		if (resourceRegistry->at(i).first == res->m_resourceType)
		{
			resourceRegistry->at(i).second->m_resource->SetResourceToDefaults(res);
			foundRegistryRes = true;
			break;
		}
	}

	if (!foundRegistryRes)
		LogFatalError("Couldnt find resource type in Registry. Has it been registered?");
}

void ResourceManager::LoadResourceDatabase(nlohmann::json resourceDatabase)
{
	m_vResourceDatabase.clear();
	m_vLoadedResources.clear();
	m_resourceDatabaseJson = resourceDatabase;

	NobleRegistry* registry = Application::GetApplication()->GetRegistry();
	std::vector<std::pair<std::string, ResourceRegistryBase*>>* resourceRegistry = registry->GetResourceRegistry();

	if (m_resourceDatabaseJson.find("Defaults") != m_resourceDatabaseJson.end())
	{
		nlohmann::json def = m_resourceDatabaseJson.at("Defaults");

		for (int i = 0; i < resourceRegistry->size(); i++)
		{
			if (def.find(resourceRegistry->at(i).first) != def.end())
			{
				resourceRegistry->at(i).second->m_resource->SetDefaults(def[resourceRegistry->at(i).first]);
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
				std::shared_ptr<Resource> res = resourceRegistry->at(i).second->m_resource->LoadFromJson(it.key(), it.value());
				m_vResourceDatabase.push_back(res);
			}

			LogInfo(FormatString("Loaded %d %s", ac.size(), resourceRegistry->at(i).first.c_str()));
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

	NobleRegistry* registry = Application::GetApplication()->GetRegistry();
	std::vector<std::pair<std::string, ResourceRegistryBase*>>* resourceRegistry = registry->GetResourceRegistry();
	for (int i = 0; i < resourceRegistry->size(); i++)
	{
		m_resourceDatabaseJson["Defaults"][resourceRegistry->at(i).first] = resourceRegistry->at(i).second->m_resource->AddToDatabase();
	}
	return m_resourceDatabaseJson;
}

void ResourceManager::ScanForResources()
{
	if(Application::GetApplication()->GetProjectFile() == nullptr)
		return;

	std::vector<std::string> files = GetAllFiles(GetGameDataFolder(), true);

	for (size_t i = 0; i < files.size(); i++)
	{
		std::string type = GetResourceTypeFromPath(files.at(i));
		if (type == "NotSupported")
			continue;

		if (IsFileInDatabase(type, GetFolderLocationRelativeToGameData(files.at(i))))
			continue;

		LogInfo("Found new resource " + files.at(i) + " of type " + type + " that is not in the database.");

		AddNewResource(type, files.at(i));
	}

	NobleRegistry* registry = Application::GetApplication()->GetRegistry();
	std::vector<std::pair<std::string, ResourceRegistryBase*>>* resourceRegistry = registry->GetResourceRegistry();
	for (int o = 0; o < resourceRegistry->size(); o++)
	{
		if (m_resourceDatabaseJson.find(resourceRegistry->at(o).first) != m_resourceDatabaseJson.end())
		{
			if (resourceRegistry->at(o).first == "Pipeline") //skip these as they are currently non file based
				continue;

			nlohmann::json ac = m_resourceDatabaseJson.at(resourceRegistry->at(o).first);
			for (auto it : ac.items())
			{
				bool foundResource = false;

				for (size_t i = 0; i < files.size(); i++)
				{
					if (files.at(i) == "")
						continue;

					if (GetFolderLocationRelativeToGameData(files.at(i)) == it.key())
					{
						foundResource = true;
						files.erase(files.begin() + i);
						break;
					}
				}

				if (foundResource == false)
				{
					RemoveResourceFromDatabase(it.key());
				}
			}
		}
	}
}

bool ResourceManager::IsFileInDatabase(std::string type, std::string path)
{
	nlohmann::json typeJson = m_resourceDatabaseJson[type];
	bool isInDatabase = (typeJson.find(path) != typeJson.end());
	return isInDatabase;
}

std::string ResourceManager::GetResourceTypeFromPath(std::string path)
{
	NobleRegistry* registry = Application::GetApplication()->GetRegistry();
	std::vector<std::pair<std::string, ResourceRegistryBase*>>* resourceRegistry = registry->GetResourceRegistry();

	std::vector<std::string> splitPath = SplitString(path, '.');
	std::string fileType = splitPath.at(splitPath.size() - 1);

	for (int i = 0; i < resourceRegistry->size(); i++)
	{
		if (resourceRegistry->at(i).second->m_sAcceptedFileTypes.find(fileType) != std::string::npos)
		{
			return resourceRegistry->at(i).first;
		}
	}

	return "NotSupported";
}

std::vector<std::shared_ptr<Resource>> ResourceManager::GetAllResourcesOfType(std::string type)
{
	std::vector<std::shared_ptr<Resource>> returnVec;

	for (int i = 0; i < m_vResourceDatabase.size(); i++)
	{
		if (m_vResourceDatabase.at(i)->m_resourceType == type)
			returnVec.push_back(m_vResourceDatabase.at(i));
	}

	return returnVec;
}

void ResourceManager::UnloadUnusedResources()
{
	if (m_vLoadedResources.empty())
		return;

	for (size_t re = m_vLoadedResources.size()-1; re > 0 ; re--)
	{
		if (m_vLoadedResources.at(re).use_count() == 2)
		{
			LogInfo("Unloaded " + m_vLoadedResources.at(re)->m_sLocalPath);

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

std::shared_ptr<Resource> ResourceManager::DoResourceSelectInterface(std::string interfaceText, std::string currentResourcePath, std::string type)
{
	std::vector<std::shared_ptr<Resource>> resources = GetAllResourcesOfType(type);

	if (resources.size() == 0)
	{
		ImGui::Text(FormatString("No resources of type %s exist in database.", interfaceText).c_str());
		return nullptr;
	}

	static std::string searchVal = "";
	std::vector<std::shared_ptr<Resource>> displayResources = resources;

	int res = -1;

	ImGui::Text(interfaceText.c_str());
	ImGui::SameLine();
	if (ImGui::BeginMenu(currentResourcePath.c_str()))
	{
		ImGui::InputText("Search", &searchVal);

		for (int i = displayResources.size() - 1; i >= 0; i--)
		{
			if (!searchVal.empty())
			{
				if (displayResources.at(i)->m_sLocalPath.find(searchVal) == std::string::npos)
				{
					displayResources.erase(displayResources.begin() + i);
					continue;
				}
			}
		}

		if (displayResources.size() == 0)
		{
			ImGui::Text(FormatString("No resources found.", interfaceText).c_str());
			ImGui::EndMenu();
			return nullptr;
		}

		for (int i = 0; i < displayResources.size(); i++)
		{
			if (ImGui::MenuItem(displayResources.at(i)->m_sLocalPath.c_str()))
				res = i;
		}

		ImGui::EndMenu();
	}
	ImGui::Dummy(ImVec2(0.0f, 5.0f));

	if (res != -1)
	{
		if (displayResources.at(res)->m_sLocalPath != currentResourcePath)
		{
			return LoadResource<Resource>(displayResources.at(res)->m_sLocalPath);
		}
	}

	return nullptr;
}