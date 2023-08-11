#pragma once

#include<map>
#include<string>

#include "../Resource/Resource.h"

class NobleRegistry
{
	static std::map<int, std::pair<std::string, Resource*>> m_mResourceRegistry;

public:

	static void RegisterResource(std::string ID, Resource* resource);
	static std::map<int, std::pair<std::string, Resource*>>* GetResourceRegistry() { return &m_mResourceRegistry; }
};