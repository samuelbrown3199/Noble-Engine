#include "ResourceManager.h"
#include "../Useful.h"

std::vector<std::shared_ptr<Resource>> ResourceManager::m_vResources;
std::string ResourceManager::m_sWorkingDirectory;

ResourceManager::ResourceManager()
{
	m_sWorkingDirectory = GetWorkingDirectory();
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