#include "Registry.h"

std::map<int, std::pair<std::string, Resource*>> NobleRegistry::m_mResourceRegistry;
std::map<int, std::pair<std::string, ComponentRegistry>> NobleRegistry::m_mComponentRegistry;
std::map<int, std::pair<std::string, Behaviour*>> NobleRegistry::m_mBehaviourRegistry;

void NobleRegistry::RegisterResource(std::string ID, Resource* resource)
{
	m_mResourceRegistry[m_mResourceRegistry.size()] = std::make_pair(ID, resource);
}

Datalist* NobleRegistry::GetComponentList(std::string ID)
{
	for (int i = 0; i < m_mComponentRegistry.size(); i++)
	{
		if (m_mComponentRegistry.at(i).first == ID)
			return m_mComponentRegistry.at(i).second.m_componentDatalist;
	}

	Logger::LogError(FormatString("Trying to get component list %s, but it hasn't been registered.", ID.c_str()), 2);
}

void NobleRegistry::RegisterBehaviour(std::string ID, Behaviour* comp)
{
	m_mBehaviourRegistry[m_mBehaviourRegistry.size()] = std::make_pair(ID, comp);
}