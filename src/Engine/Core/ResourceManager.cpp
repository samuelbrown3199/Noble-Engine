#include "ResourceManager.h"
#include "../Useful.h"

std::vector<std::shared_ptr<Resource>> ResourceManager::m_vResources;
std::string ResourceManager::m_sWorkingDirectory;

FT_Library ResourceManager::m_fontLibrary;

ResourceManager::ResourceManager()
{
	m_sWorkingDirectory = GetWorkingDirectory();

	if (FT_Init_FreeType(&m_fontLibrary))
	{
		Logger::LogError("Could not initialize FreeType Library!", 2);
		throw std::exception();
	}
}

void ResourceManager::UnloadUnusedResources()
{
	for (size_t re = 0; re < m_vResources.size(); re++)
	{
		if (m_vResources.at(re).use_count() == 1)
		{
			m_vResources.erase(m_vResources.begin() + re);
		}
	}
}