#include "ResourceManager.h"
#include "../Useful.h"

std::vector<std::shared_ptr<Resource>> ResourceManager::m_vResources;
std::vector<std::shared_ptr<ShaderProgram>> ResourceManager::m_vShaderPrograms;
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

std::shared_ptr<Font> ResourceManager::LoadFont(std::string fontPath, int fontPixelSize)
{
	for (size_t re = 0; re < m_vResources.size(); re++)
	{
		if (m_vResources.at(re)->m_sResourcePath == fontPath)
		{
			std::shared_ptr<Font> resource = std::dynamic_pointer_cast<Font>(m_vResources.at(re));
			if (resource)
			{
				if (resource->fontPixelSize == fontPixelSize)
				{
					return resource;
				}
			}
		}
	}

	std::shared_ptr<Font> newResource = std::make_shared<Font>(fontPixelSize);
	newResource->m_sResourcePath = fontPath;
	newResource->OnLoad();
	m_vResources.push_back(newResource);
	return newResource;
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