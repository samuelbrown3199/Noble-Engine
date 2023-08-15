#include "Registry.h"

std::map<int, std::pair<std::string, Resource*>> NobleRegistry::m_mResourceRegistry;
std::map<int, std::pair<std::string, ComponentRegistry>> NobleRegistry::m_mComponentRegistry;
std::map<int, std::pair<std::string, Behaviour*>> NobleRegistry::m_mBehaviourRegistry;

void NobleRegistry::RegisterResource(std::string ID, Resource* resource)
{
	m_mResourceRegistry[m_mResourceRegistry.size()] = std::make_pair(ID, resource);
}

void NobleRegistry::RegisterComponent(std::string ID, Component* comp, bool useThreads, int maxComponentsPerThread)
{
	ComponentRegistry reg(comp, useThreads, maxComponentsPerThread);
	m_mComponentRegistry[m_mComponentRegistry.size()] = std::make_pair(ID, reg);
}

void NobleRegistry::RegisterBehaviour(std::string ID, Behaviour* comp)
{
	m_mBehaviourRegistry[m_mBehaviourRegistry.size()] = std::make_pair(ID, comp);
}