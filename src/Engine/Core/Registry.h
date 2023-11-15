#pragma once

#include <map>
#include <memory>
#include <string>
#include <typeinfo>

#include "Application.h"
#include "../ECS/Component.h"
#include "../ECS/ComponentList.hpp"
#include "../Core/EngineResources/Resource.h"

struct ComponentRegistry
{	
	Component* m_comp = nullptr;
	bool m_bUseThreads = false;
	int m_iMaxComponentsPerThread = 1024;

	bool m_bUpdateInEditor, m_bRenderInEditor;

	Datalist* m_componentDatalist;

	ComponentRegistry()
	{
		m_comp = nullptr;
		m_bUseThreads = false;
		m_iMaxComponentsPerThread = 1024;
	}

	ComponentRegistry(Component* comp, Datalist* compList, bool useThreads, int maxComponentsPerThread, bool updateEditMode, bool renderEditMode)
	{
		m_comp = comp;
		m_componentDatalist = compList;
		m_bUseThreads = useThreads;
		m_iMaxComponentsPerThread = maxComponentsPerThread;
		m_bUpdateInEditor = updateEditMode;
		m_bRenderInEditor = renderEditMode;
	}
};

class NobleRegistry
{
	static std::map<int, std::pair<std::string, Resource*>> m_mResourceRegistry;
	static std::map<int, std::pair<std::string, ComponentRegistry>> m_mComponentRegistry;
	static std::map<int, std::pair<std::string, Behaviour*>> m_mBehaviourRegistry;

public:

	static void RegisterResource(std::string ID, Resource* resource);
	static std::map<int, std::pair<std::string, Resource*>>* GetResourceRegistry() { return &m_mResourceRegistry; }

	template<typename T>
	static void RegisterComponent(std::string ID, bool useThreads, int maxComponentsPerThread, bool updateEditMode, bool renderEditMode)
	{
		T* comp = new T();
		ComponentDatalist<T>* complist = new ComponentDatalist<T>();

		ComponentRegistry reg(comp, complist, useThreads, maxComponentsPerThread, updateEditMode, renderEditMode);
		m_mComponentRegistry[m_mComponentRegistry.size()] = std::make_pair(ID, reg);
	}
	static std::map<int, std::pair<std::string, ComponentRegistry>>* GetComponentRegistry() { return &m_mComponentRegistry; }
	static Datalist* GetComponentList(std::string ID);
	template<typename T>
	static int GetComponentIndex(std::string entityID)
	{
		T temp;

		for (int i = 0; i < m_mComponentRegistry.size(); i++)
		{
			if (temp.GetComponentID() == m_mComponentRegistry.at(i).second.m_comp->GetComponentID())
			{
				return m_mComponentRegistry.at(i).second.m_componentDatalist->GetComponentIndex(entityID);
			}
		}

		return -1;
	}
	template<typename T>
	static T* GetComponent(int index)
	{
		T temp;

		for (int i = 0; i < m_mComponentRegistry.size(); i++)
		{
			if (temp.GetComponentID() == m_mComponentRegistry.at(i).second.m_comp->GetComponentID())
			{
				return dynamic_cast<T*>(m_mComponentRegistry.at(i).second.m_componentDatalist->GetComponent(index));
			}
		}

		return nullptr;
	}

	static void RegisterBehaviour(std::string ID, Behaviour* comp);
	static std::map<int, std::pair<std::string, Behaviour*>>* GetBehaviourRegistry() { return &m_mBehaviourRegistry; }
};