#include "Script.h"

#include <fstream>
#include <sstream>

#include "../../Core/ResourceManager.h"

Script::Script()
{
	m_resourceType = "Script";
}

void Script::OnLoad()
{
	Resource::OnLoad();

	std::ifstream scriptFile(m_sResourcePath);
	if (scriptFile.is_open())
	{
		std::stringstream buffer;
		buffer << scriptFile.rdbuf();

		m_sScriptString = buffer.str();
	}
}

void Script::OnUnload()
{
	m_sScriptString.clear();
}

void Script::AddResource(std::string path)
{
	ResourceManager::AddNewResource<Script>(path);
}

std::vector<std::shared_ptr<Resource>> Script::GetResourcesOfType()
{
	return ResourceManager::GetAllResourcesOfType<Script>();
}

std::shared_ptr<Resource> Script::LoadFromJson(const std::string& path, const nlohmann::json& data)
{
	std::shared_ptr<Script> res = std::make_shared<Script>();

	res->m_sLocalPath = path;
	res->m_sResourcePath = GetGameFolder() + path;

	return res;
}