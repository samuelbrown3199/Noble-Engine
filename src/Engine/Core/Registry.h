#pragma once

#include <map>
#include <memory>
#include <string>
#include <typeinfo>

#include "Application.h"
#include "../ECS/Component.hpp"
#include "../Resource/Resource.h"

struct ComponentRegistry
{
	Component* m_comp = nullptr;
	bool m_bUseThreads = false;
	int m_iMaxComponentsPerThread = 1024;

	ComponentRegistry()
	{
		m_comp = nullptr;
		m_bUseThreads = false;
		m_iMaxComponentsPerThread = 1024;
	}

	ComponentRegistry(Component* comp, bool useThreads, int maxComponentsPerThread)
	{
		m_comp = comp;
		m_bUseThreads = useThreads;
		m_iMaxComponentsPerThread = maxComponentsPerThread;
	}
};

class NobleRegistry
{
	static std::map<int, std::pair<std::string, Resource*>> m_mResourceRegistry;
	static std::map<int, std::pair<std::string, ComponentRegistry>> m_mComponentRegistry;

public:

	static void RegisterResource(std::string ID, Resource* resource);
	static std::map<int, std::pair<std::string, Resource*>>* GetResourceRegistry() { return &m_mResourceRegistry; }

	static void RegisterComponent(std::string ID, Component* comp, bool useThreads, int maxComponentsPerThread);
	static std::map<int, std::pair<std::string, ComponentRegistry>>* GetComponentRegistry() { return &m_mComponentRegistry; }
};