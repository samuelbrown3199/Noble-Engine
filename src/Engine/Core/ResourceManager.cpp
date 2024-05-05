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

	registry->RegisterResource("AudioClip", new AudioClip(), true, ".wav|.ogg|.mp3");
	registry->RegisterResource("Texture", new Texture(), true, ".jpg|.png|.tga|.bmp|.hdr");
	registry->RegisterResource("Model", new Model(), true, ".obj");
	registry->RegisterResource("Pipeline", new Pipeline(), false, ".npl");
	registry->RegisterResource("Script", new Script(), true, ".lua");
	registry->RegisterResource("Shader", new Shader(), true, ".vert|.frag|.comp");
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
			LogInfo(FormatString("Remove asset %s from Resource Database", path.c_str()));
			return;
		}
	}
}

void ResourceManager::SetResourceToDefaults(std::shared_ptr<Resource> res)
{
	NobleRegistry* registry = Application::GetApplication()->GetRegistry();
	std::vector<std::pair<std::string, ResourceRegistry>>* resourceRegistry = registry->GetResourceRegistry();

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
		LogFatalError("Couldnt find resource type in Registry. Has it been registered?");
}

void ResourceManager::LoadResourceDatabase(nlohmann::json resourceDatabase)
{
	m_vResourceDatabase.clear();
	m_resourceDatabaseJson = resourceDatabase;

	NobleRegistry* registry = Application::GetApplication()->GetRegistry();
	std::vector<std::pair<std::string, ResourceRegistry>>* resourceRegistry = registry->GetResourceRegistry();

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
	std::vector<std::pair<std::string, ResourceRegistry>>* resourceRegistry = registry->GetResourceRegistry();
	for (int i = 0; i < resourceRegistry->size(); i++)
	{
		m_resourceDatabaseJson["Defaults"][resourceRegistry->at(i).first] = resourceRegistry->at(i).second.m_resource->AddToDatabase();
	}
	return m_resourceDatabaseJson;
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
			return displayResources.at(res);
	}

	return nullptr;
}