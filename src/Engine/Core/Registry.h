#pragma once

#include <map>
#include <memory>
#include <string>
#include <typeinfo>

#include "Application.h"
#include "Graphics/Renderer.h"
#include "ECS/Component.h"
#include "ECS/ComponentList.hpp"
#include "EngineResources/Resource.h"

struct ComponentRegistry
{	
	Component* m_comp = nullptr;
	bool m_bUseThreads = false;	
	int m_iMaxComponentsPerThread = 1024;

	bool m_bUpdateInEditor = false, m_bRenderInEditor = false;

	Datalist* m_componentDatalist = nullptr;

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

struct ResourceRegistry
{
	Resource* m_resource = nullptr;
	bool m_bRequiresFile = true;
};

struct PushConstantRegistry
{
	VkPushConstantRange m_pushConstantRange;

	PushConstantRegistry(VkPushConstantRange constantRange)
	{
		m_pushConstantRange = constantRange;
	}
};

struct DescriptorRegistry
{
	VkDescriptorSetLayout* m_layout;
	VkDescriptorSet* m_set = nullptr;
	VkDescriptorType m_type;

	DescriptorRegistry(VkDescriptorSetLayout* layout, VkDescriptorSet* set, VkDescriptorType type)
	{
		m_layout = layout;
		m_set = set;
		m_type = type;
	}
};

class NobleRegistry
{

	//change these to vectors at some point, not sure why I made them maps.
	static std::map<int, std::pair<std::string, ResourceRegistry>> m_mResourceRegistry;
	static std::map<int, std::pair<std::string, ComponentRegistry>> m_mComponentRegistry;
	static std::map<int, std::pair<std::string, Behaviour*>> m_mBehaviourRegistry;

	std::vector<std::pair<std::string, PushConstantRegistry>> m_vPushConstantRegistry;
	std::vector<std::pair<std::string, DescriptorRegistry>> m_vDescriptorRegistry;

public:

	static void RegisterResource(std::string ID, Resource* resource, bool requiresFile);
	static std::map<int, std::pair<std::string, ResourceRegistry>>* GetResourceRegistry() { return &m_mResourceRegistry; }

	template<typename T>
	static void RegisterComponent(std::string ID, bool useThreads, int maxComponentsPerThread, bool updateEditMode, bool renderEditMode)
	{
		T* comp = new T();
		ComponentDatalist<T>* complist = new ComponentDatalist<T>();

		ComponentRegistry reg(comp, complist, useThreads, maxComponentsPerThread, updateEditMode, renderEditMode);
		m_mComponentRegistry[(int)m_mComponentRegistry.size()] = std::make_pair(ID, reg);

		PerformanceStats* pStats = Application::GetPerformanceStats();
		pStats->AddComponentMeasurement(ID);
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

	void RegisterDescriptor(std::string ID, VkDescriptorSetLayout* layout, VkDescriptorSet* set, VkDescriptorType type);
	std::vector<std::pair<std::string, DescriptorRegistry>>* GetDescriptorRegistry() { return &m_vDescriptorRegistry; }
	DescriptorRegistry* GetDescriptorFromName(std::string ID);

	template<typename T>
	void RegisterPushConstant(std::string ID, VkShaderStageFlagBits shaderStageFlags)
	{
		VkPushConstantRange bufferRange{};
		bufferRange.offset = 0;
		bufferRange.size = sizeof(T);
		bufferRange.stageFlags = shaderStageFlags;

		m_vPushConstantRegistry.push_back(std::make_pair(ID, PushConstantRegistry(bufferRange)));
	}
	std::vector<std::pair<std::string, PushConstantRegistry>>* GetPushConstantRegistry() { return &m_vPushConstantRegistry; }
	PushConstantRegistry* GetPushConstantFromName(std::string ID);
};