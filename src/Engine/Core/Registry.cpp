#include "Registry.h"

std::map<int, std::pair<std::string, Resource*>> NobleRegistry::m_mResourceRegistry;

void NobleRegistry::RegisterResource(std::string ID, Resource* resource)
{
	m_mResourceRegistry[m_mResourceRegistry.size()] = std::make_pair(ID, resource);
}