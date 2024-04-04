#include "Registry.h"

std::map<int, std::pair<std::string, ResourceRegistry>> NobleRegistry::m_mResourceRegistry;
std::map<int, std::pair<std::string, ComponentRegistry>> NobleRegistry::m_mComponentRegistry;
std::map<int, std::pair<std::string, Behaviour*>> NobleRegistry::m_mBehaviourRegistry;

void NobleRegistry::RegisterResource(std::string ID, Resource* resource, bool requiresFile)
{
	ResourceRegistry newRegistry;
	newRegistry.m_resource = resource;
	newRegistry.m_bRequiresFile = requiresFile;

	m_mResourceRegistry[(int)m_mResourceRegistry.size()] = std::make_pair(ID, newRegistry);
}

Datalist* NobleRegistry::GetComponentList(std::string ID)
{
	for (int i = 0; i < m_mComponentRegistry.size(); i++)
	{
		if (m_mComponentRegistry.at(i).first == ID)
			return m_mComponentRegistry.at(i).second.m_componentDatalist;
	}

	Logger::LogError(FormatString("Trying to get component list %s, but it hasn't been registered.", ID.c_str()), 2);
	return nullptr;
}

void NobleRegistry::RegisterBehaviour(std::string ID, Behaviour* comp)
{
	m_mBehaviourRegistry[(int)m_mBehaviourRegistry.size()] = std::make_pair(ID, comp);
}

void NobleRegistry::RegisterDescriptor(std::string ID, VkDescriptorSetLayout* layout, VkDescriptorSet* set, VkDescriptorType type)
{
	m_vDescriptorRegistry.push_back(std::make_pair(ID, DescriptorRegistry(layout, set, type)));
}

DescriptorRegistry* NobleRegistry::GetDescriptorFromName(std::string ID)
{
	for (int i = 0; i < m_vDescriptorRegistry.size(); i++)
	{
		if (m_vDescriptorRegistry.at(i).first == ID)
			return &m_vDescriptorRegistry.at(i).second;
	}

	Logger::LogError(FormatString("Failed to get Descriptor with ID %s", ID.c_str()), 2);
	return nullptr;
}

PushConstantRegistry* NobleRegistry::GetPushConstantFromName(std::string ID)
{
	for (int i = 0; i < m_vPushConstantRegistry.size(); i++)
	{
		if (m_vPushConstantRegistry.at(i).first == ID)
			return &m_vPushConstantRegistry.at(i).second;
	}

	Logger::LogError(FormatString("Failed to get Push Constant with ID %s", ID.c_str()), 2);
	return nullptr;
}