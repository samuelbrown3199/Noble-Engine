#pragma once

#include <vector>
#include <memory>
#include <deque>

#include <nlohmann/json.hpp>

#include "../imgui/imgui.h"

template <typename T>
struct ComponentDatalist
{
	std::vector<T> m_componentData;
	std::deque<T*> m_deletedComponents;

	void ComponentDatalist::AddComponent(T* comp)
	{
		if (!m_deletedComponents.empty())
		{
			T* co = m_deletedComponents.front();
			*co = *comp;

			m_deletedComponents.pop_front();

			return;
		}

		m_componentData.push_back(*comp);
	}

	void ComponentDatalist::RemoveComponent(std::string entityID)
	{
		for (int i = 0; i < m_componentData.size(); i++)
		{
			if (m_componentData.at(i).m_sEntityID == entityID)
			{
				m_componentData.at(i).m_sEntityID = "del";
				m_deletedComponents.push_back(&m_componentData.at(i));
			}
		}
	}

	void ComponentDatalist::RemoveAllComponents()
	{
		m_componentData.clear();
		m_deletedComponents.clear();
	}

	T* ComponentDatalist::GetComponent(std::string entityID)
	{
		for (int i = 0; i < m_componentData.size(); i++)
		{
			if (m_componentData.at(i).m_sEntityID == entityID)
				return &m_componentData.at(i);
		}

		return nullptr;
	}

	void ComponentDatalist::Update(bool useThreads, int maxComponentsPerThread)
	{
		if (!useThreads)
		{
			for (int i = 0; i < m_componentData.size(); i++)
			{
				m_componentData.at(i).OnUpdate();
			}
		}
		else
		{
			double amountOfThreads = ceil(m_componentData.size() / maxComponentsPerThread) + 1;
			for (int i = 0; i < amountOfThreads; i++)
			{
				int buffer = maxComponentsPerThread * i;
				auto th = ThreadingManager::EnqueueTask([&] { ThreadUpdate(buffer, maxComponentsPerThread); });
			}
		}
	}

	void ComponentDatalist::ThreadUpdate(int _buffer, int _amount)
	{
		int maxCap = _buffer + _amount;
		for (size_t co = _buffer; co < maxCap; co++)
		{
			if (co >= m_componentData.size())
				break;

			m_componentData.at(co).OnUpdate();
		}
	}

	void ComponentDatalist::Render(bool useThreads, int maxComponentsPerThread)
	{
		if (!useThreads)
		{
			for (int i = 0; i < m_componentData.size(); i++)
			{
				m_componentData.at(i).OnRender();
			}
		}
		else
		{
			double amountOfThreads = ceil(m_componentData.size() / maxComponentsPerThread) + 1;
			for (int i = 0; i < amountOfThreads; i++)
			{
				int buffer = maxComponentsPerThread * i;
				auto th = ThreadingManager::EnqueueTask([&] { ThreadRender(buffer, maxComponentsPerThread); });
			}
		}
	}

	void ComponentDatalist::ThreadRender(int _buffer, int _amount)
	{
		int maxCap = _buffer + _amount;
		for (size_t co = _buffer; co < maxCap; co++)
		{
			if (co >= m_componentData.size())
				break;

			m_componentData.at(co).OnRender();
		}
	}

	void ComponentDatalist::LoadComponentDataFromJson(nlohmann::json& j)
	{
		for (auto it : j.items())
		{
			T component;
			component.m_sEntityID = it.key();
			component.FromJson(j[it.key()]);

			component.AddComponent();
		}
	}

	nlohmann::json ComponentDatalist::WriteComponentDataToJson()
	{
		nlohmann::json data;

		for (int i = 0; i < m_componentData.size(); i++)
		{
			data[m_componentData.at(i).m_sEntityID] = m_componentData.at(i).WriteJson();
		}

		return data;
	}
};

struct Component
{
	/**
	*Stores the entity ID for the entity this component belongs to.
	*/
	std::string m_sEntityID;

	/**
	* This can be used to create initialisation functions for components.
	*/
	virtual void OnInitialize() {};
	/**
	* This is used when deleting component data. Useful for cleaning up memory etc.
	*/
	virtual void OnRemove() {};

	virtual nlohmann::json WriteJson() { nlohmann::json data; return data; };
	virtual void FromJson(const nlohmann::json& j) {}

	virtual void DoComponentInterface()
	{
		ImGui::Text("No editable values. Component window not defined for this type.");
	}

	virtual Component* GetAsComponent(std::string entityID) = 0;
	virtual void AddComponent() = 0;
	virtual void RemoveComponent(std::string entityID) = 0;
	virtual void RemoveAllComponents() = 0;

	virtual void PreUpdate() {}
	virtual void Update(bool useThreads, int maxComponentsPerThread) = 0;
	virtual void OnUpdate() {}

	virtual void PreRender() {};
	virtual void Render(bool useThreads, int maxComponentsPerThread) = 0;
	virtual void OnRender() {}

	virtual void LoadComponentDataFromJson(nlohmann::json& j) = 0;
	virtual nlohmann::json WriteComponentDataToJson() = 0;
};