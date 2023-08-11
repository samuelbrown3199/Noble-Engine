#include "Script.h"

#include <fstream>
#include <sstream>

#include "../Core/ResourceManager.h"

Script::Script()
{
	m_resourceType = "Script";
}

void Script::OnLoad()
{
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
