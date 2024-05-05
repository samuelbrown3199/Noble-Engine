#include "Registry.h"

Datalist* NobleRegistry::GetComponentList(std::string ID)
{
	for (int i = 0; i < m_vComponentRegistry.size(); i++)
	{
		if (m_vComponentRegistry.at(i).first == ID)
			return m_vComponentRegistry.at(i).second.m_componentDatalist;
	}

	LogFatalError(FormatString("Trying to get component list %s, but it hasn't been registered.", ID.c_str()));
	return nullptr;
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

	LogFatalError(FormatString("Failed to get Descriptor with ID %s", ID.c_str()));
	return nullptr;
}

PushConstantRegistry* NobleRegistry::GetPushConstantFromName(std::string ID)
{
	for (int i = 0; i < m_vPushConstantRegistry.size(); i++)
	{
		if (m_vPushConstantRegistry.at(i).first == ID)
			return &m_vPushConstantRegistry.at(i).second;
	}

	LogFatalError(FormatString("Failed to get Push Constant with ID %s", ID.c_str()));
	return nullptr;
}

void NobleRegistry::ClearRegistries()
{
	for (int i = 0; i < m_vComponentRegistry.size(); i++)
	{
		delete m_vComponentRegistry.at(i).second.m_componentDatalist;
		delete m_vComponentRegistry.at(i).second.m_comp;
	}
	m_vComponentRegistry.clear();

	m_vResourceRegistry.clear();
	m_vDescriptorRegistry.clear();
	m_vPushConstantRegistry.clear();
}