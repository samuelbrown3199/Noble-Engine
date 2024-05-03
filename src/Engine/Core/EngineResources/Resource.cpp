#include "Resource.h"

#include "../Application.h"

std::vector<std::shared_ptr<Resource>> Resource::GetResourcesOfType()
{
	ResourceManager* rManager = Application::GetApplication()->GetResourceManager();
	return rManager->GetAllResourcesOfType(m_resourceType);
}