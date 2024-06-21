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
	UnloadAllResources();
}

void ResourceManager::RegisterResourceTypes()
{
	NobleRegistry* registry = Application::GetApplication()->GetRegistry();

	registry->RegisterResource<AudioClip>("AudioClip", true, ".wav,.ogg,.mp3");
	registry->RegisterResource<Texture>("Texture", true, ".jpg,.png,.tga,.bmp,.hdr");
	registry->RegisterResource<Model>("Model", true, ".obj");
	registry->RegisterResource<Pipeline>("Pipeline", false, "");
	registry->RegisterResource<Script>("Script", true, ".lua");
	registry->RegisterResource<Shader>("Shader", true, ".vert,.frag,.comp");
}

void ResourceManager::SetWorkingDirectory(std::string directory)
{
	m_sWorkingDirectory = directory;
}

void ResourceManager::AddNewResource(std::string type, std::string path)
{
	if (type == "NotSupported")
	{
		LogError("Tried to add a resource of type NotSupported.");
		return;
	}

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

	m_mResourceDatabase[res->m_sLocalPath] = res;
	LogInfo(FormatString("Added new resource %s", res->m_sLocalPath.c_str()));
}

void ResourceManager::RemoveResourceFromDatabase(std::string localPath)
{
	m_mResourceDatabase.erase(localPath);
	LogInfo(FormatString("Remove asset %s from Resource Database", localPath.c_str()));
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
	std::lock_guard<std::mutex> lock(m_resourceDatabaseMutex);

	LogInfo("Loading Resource Database");

	std::map<std::string, std::shared_ptr<Resource>> loadedResources;
	loadedResources = m_mLoadedResources;

	m_mResourceDatabase.clear();
	m_mLoadedResources.clear();
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
				std::shared_ptr<Resource> res;
				res = PrelimLoadResource<Resource>(it.key(), loadedResources);
				if (res == nullptr)
				{
					res = resourceRegistry->at(i).second->m_resource->LoadFromJson(it.key(), it.value());
				}
				else
				{
					m_mLoadedResources[it.key()] = res;
					loadedResources.erase(it.key());
				}

				m_mResourceDatabase[it.key()] = res;
				std::chrono::file_time fileTime = std::filesystem::file_time_type::min();
				if(PathExists(res->m_sResourcePath))
					fileTime = std::filesystem::last_write_time(res->m_sResourcePath);

				m_vPreviousScanFiles[res->m_sResourcePath] = fileTime;
			}

			LogInfo(FormatString("Loaded %d %s into database", ac.size(), resourceRegistry->at(i).first.c_str()));
		}
	}

	LogInfo("Resource Database Loaded");
}

nlohmann::json ResourceManager::WriteResourceDatabase()
{
	m_resourceDatabaseJson.clear();

	std::map<std::string, std::shared_ptr<Resource>>::iterator it = m_mResourceDatabase.begin();
	for (; it != m_mResourceDatabase.end(); it++)
	{
		m_resourceDatabaseJson[it->second->m_resourceType][it->first] = it->second->AddToDatabase();
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

	std::map<std::string, std::filesystem::file_time_type> files = GetAllFilesAndLastWriteTime(GetGameDataFolder(), true);
	NobleRegistry* registry = Application::GetApplication()->GetRegistry();
	std::vector<std::pair<std::string, ResourceRegistryBase*>>* resourceRegistry = registry->GetResourceRegistry();

	if (files == m_vPreviousScanFiles)
		return;

	//get a list of added files
	std::map<std::string, std::filesystem::file_time_type> addedFiles;
	std::map<std::string, std::filesystem::file_time_type>::iterator it = files.begin();
	for (; it != files.end(); it++)
	{
		if (m_vPreviousScanFiles.find(it->first) == m_vPreviousScanFiles.end())
		{
			addedFiles[it->first] = it->second;
		}
	}

	//get a list of removed files
	std::map<std::string, std::filesystem::file_time_type> removedFiles;
	it = m_vPreviousScanFiles.begin();
	for (; it != m_vPreviousScanFiles.end(); it++)
	{
		if (files.find(it->first) == files.end())
		{
			removedFiles[it->first] = it->second;
		}
	}

	//get list of modified files
	std::map<std::string, std::filesystem::file_time_type> modifiedFiles;
	it = files.begin();
	for (; it != files.end(); it++)
	{
		if (m_vPreviousScanFiles.find(it->first) != m_vPreviousScanFiles.end())
		{
			if (it->second != m_vPreviousScanFiles.at(it->first))
			{
				modifiedFiles[it->first] = it->second;
			}
		}
	}

	//most of the slowdown is in this loop.
	it = addedFiles.begin();
	for (; it != addedFiles.end(); it++)
	{
		std::string type = GetResourceTypeFromPath(it->first);

		if(type == "NotSupported")
			continue;

		LogInfo("Found new resource " + it->first + " of type " + type + " that is not in the database.");
		AddNewResource(type, it->first);
	}

	for (int o = 0; o < resourceRegistry->size(); o++)
	{
		if (m_resourceDatabaseJson.find(resourceRegistry->at(o).first) != m_resourceDatabaseJson.end())
		{
			//skip any resources that dont require a file
			if (!resourceRegistry->at(o).second->m_bRequiresFile)
				continue;

			nlohmann::json ac = m_resourceDatabaseJson.at(resourceRegistry->at(o).first);
			for (auto it : ac.items())
			{
				bool foundResource = true;
				std::map<std::string, std::filesystem::file_time_type>::iterator fileItr = removedFiles.begin();
				for (; fileItr != removedFiles.end(); fileItr++)
				{
					if (fileItr->first == "")
						continue;

					if (GetFolderLocationRelativeToGameData(fileItr->first) == it.key())
					{
						foundResource = false;
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

	it = modifiedFiles.begin();
	for (; it != modifiedFiles.end(); it++)
	{
		std::string type = GetResourceTypeFromPath(it->first);

		if(type == "NotSupported")
			continue;

		std::shared_ptr<Resource> res = GetResourceFromDatabase<Resource>(GetFolderLocationRelativeToGameData(it->first), true);
		if (res != nullptr && res->IsLoaded())
		{
			m_qReloadQueue.push_back(res);
			LogInfo("Found modified resource " + it->first + ", pushed into the reload queue");
		}
	}

	if(m_vPreviousScanFiles != files)
		Application::GetApplication()->GetProjectFile()->UpdateProjectFile();

	m_vPreviousScanFiles = files;
}

void ResourceManager::ReloadResources()
{
	if(m_qReloadQueue.empty())
		return;

	Renderer* renderer = Application::GetApplication()->GetRenderer();
	vkDeviceWaitIdle(renderer->GetLogicalDevice());

	std::shared_ptr<Resource> res = m_qReloadQueue.front();
	if (res != nullptr)
	{
		std::ifstream file = std::ifstream(res->m_sResourcePath);
		if (file.is_open())
		{
			file.close();

			res->ReloadResource();
			m_qReloadQueue.pop_front();
		}
	}
}

bool ResourceManager::IsFileInDatabase(std::string path)
{
	bool isInDatabase = (m_mResourceDatabase.count(path) != 0);
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
	std::lock_guard<std::mutex> lock(m_resourceDatabaseMutex);
	std::vector<std::shared_ptr<Resource>> returnVec;

	std::map<std::string, std::shared_ptr<Resource>>::iterator it = m_mResourceDatabase.begin();
	for (; it != m_mResourceDatabase.end(); it++)
	{
		if (it->second->m_resourceType == type)
			returnVec.push_back(it->second);
	}

	return returnVec;
}

void ResourceManager::UnloadUnusedResources()
{
	std::lock_guard<std::mutex> lock(m_resourceDatabaseMutex);

	if (m_mLoadedResources.empty())
		return;

	std::map<std::string, std::shared_ptr<Resource>>::iterator it;
	std::vector<std::string> keysToDelete;
	for (it = m_mLoadedResources.end(); it != m_mLoadedResources.begin(); it--)
	{
		if (it == m_mLoadedResources.end())
			continue;

		if (it->second == nullptr)
			continue;

		if (it->second.use_count() == 2)
		{
			keysToDelete.push_back(it->first);
		}
	}

	for (int i = 0; i < keysToDelete.size(); i++)
	{
		m_mLoadedResources[keysToDelete.at(i)]->OnUnload();
		m_mLoadedResources.erase(keysToDelete.at(i));
		LogInfo("Unloaded " + keysToDelete.at(i));
	}
}

void ResourceManager::UnloadAllResources()
{
	m_mLoadedResources.clear();
	m_mResourceDatabase.clear();
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

	//if current resource path is not empty, let me double click to select it
	if (currentResourcePath != "")
	{
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
		{
			Application::GetApplication()->GetEditor()->SetSelectedResource(currentResourcePath);
		}
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

float ResourceManager::GetResourceMemoryUsage()
{
	float totalMemory = 0.0f;
	std::map<std::string, std::shared_ptr<Resource>>::iterator it = m_mLoadedResources.begin();
	for (; it != m_mLoadedResources.end(); it++)
	{
		totalMemory += it->second->GetResourceSize();
	}

	return ConvertBytesToMB(totalMemory);
}

void ResourceManager::CompileShaders()
{
	LogInfo("Checking shaders for compilation.");

	//Create a shader cache folder if it doesnt exist
	std::string cacheFolder = GetGameFolder() + "\\ShaderCache";
	if (!CreateNewDirectory(cacheFolder))
	{
		LogFatalError("Could not create shader cache folder.");
		return;
	}

	std::vector<std::shared_ptr<Resource>> shaders = GetAllResourcesOfType("Shader");
	for (int i = 0; i < shaders.size(); i++)
	{
		std::shared_ptr<Shader> shader = std::dynamic_pointer_cast<Shader>(shaders.at(i));
		if (shader != nullptr)
		{
			//We need to get the source file modified time and compare it to the compiled file modified time.
			//Check if the compiled shader is out of date or doesnt exist.
			//If it is, recompile the shader.

			bool shaderNeedsRecompiling = false;

			if (!shader->m_sCompiledShaderPath.empty() && PathExists(shader->m_sCompiledShaderPath))
			{
				std::filesystem::file_time_type sourceFileTime = GetFileLastWriteTime(shader->m_sResourcePath);
				std::filesystem::file_time_type compiledFileTime = GetFileLastWriteTime(shader->m_sCompiledShaderPath);

				if (sourceFileTime > compiledFileTime)
				{
					shaderNeedsRecompiling = true;
					LogTrace("Shader " + shader->m_sLocalPath + " needs recompiling as the source file has been modified since the last compile.");
				}
			}
			else
			{
				shaderNeedsRecompiling = true;
				LogTrace("Shader " + shader->m_sLocalPath + " needs recompiling as the compiled shader does not exist.");
			}

			if (shaderNeedsRecompiling)
			{
				std::string shadername = shader->m_sCompiledShaderName.empty() ? GenerateRandomString(10) + ".spv" : shader->m_sCompiledShaderName;
				std::string compilePath = cacheFolder + "\\" + shadername;

				while(shader->m_sCompiledShaderName.empty() && PathExists(compilePath))
				{
					shadername = GenerateRandomString(10) + ".spv";
					compilePath = cacheFolder + "\\" + shadername;
				}
				shader->m_sCompiledShaderName = shadername;
				shader->m_sCompiledShaderPath = GetFolderLocationRelativeToWorkingDirectory(compilePath);

				LogInfo("Compiling shader to " + compilePath); //remove this later, this log is for debugging purposes

				//TODO: Compile the shader here
			}
		}
		else
		{
			LogFatalError("Trying to compile a resource that is not a shader.");
		}
	}

	LogInfo("Shader compilation complete.");
}