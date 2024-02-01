#pragma once

#include <nlohmann/json.hpp>

#include <vector>
#include <deque>

#include "../../Core/ThreadingManager.h"

struct Component;

struct Datalist
{
	virtual void AddComponent(Component* comp) = 0;
	virtual void AddComponentToEntity(std::string entityID) = 0;
	virtual void RemoveComponent(std::string entityID) = 0;
	virtual void RemoveAllComponents() = 0;
	virtual int GetComponentIndex(std::string entityID) = 0;
	virtual Component* GetComponent(int index) = 0;
	virtual void Update(bool useThreads, int maxComponentsPerThread) = 0;
	virtual void ThreadUpdate(int _buffer, int _amount) = 0;
	virtual void Render(bool useThreads, int maxComponentsPerThread) = 0;
	virtual void ThreadRender(int _buffer, int _amount) = 0;
	virtual void LoadComponentDataFromJson(nlohmann::json& j) = 0;
	virtual nlohmann::json WriteComponentDataToJson() = 0;
};

template <typename T>
struct ComponentDatalist : public Datalist
{
	std::vector<T> m_componentData;
	std::deque<T*> m_deletedComponents;

	void ComponentDatalist::AddComponent(Component* comp) override
	{
		if (!m_deletedComponents.empty())
		{
			T* co = m_deletedComponents.front();
			*co = *dynamic_cast<T*>(comp);

			m_deletedComponents.pop_front();

			return;
		}

		m_componentData.push_back(*dynamic_cast<T*>(comp));
	}

	void ComponentDatalist::AddComponentToEntity(std::string entityID) override
	{
		T comp;
		comp.m_sEntityID = entityID;
		comp.OnInitialize();
		comp.AddComponent();
	}

	void ComponentDatalist::RemoveComponent(std::string entityID) override
	{
		for (int i = 0; i < m_componentData.size(); i++)
		{
			if (m_componentData.at(i).m_sEntityID == entityID)
			{
				m_componentData.at(i).m_bAvailableForReuse = true;
				m_componentData.at(i).m_sEntityID = "del";
				m_componentData.at(i).OnRemove();
				m_deletedComponents.push_back(&m_componentData.at(i));
			}
		}
	}

	void ComponentDatalist::RemoveAllComponents() override
	{
		m_componentData.clear();
		m_deletedComponents.clear();
	}

	int ComponentDatalist::GetComponentIndex(std::string entityID) override
	{
		for (int i = 0; i < m_componentData.size(); i++)
		{
			if (m_componentData.at(i).m_sEntityID == entityID)
				return i;
		}

		return -1;
	}

	Component* ComponentDatalist::GetComponent(int index) override
	{
		if (index > m_componentData.size() - 1 || index < 0)
			return nullptr;

		return &m_componentData.at(index);
	}

	void ComponentDatalist::Update(bool useThreads, int maxComponentsPerThread) override
	{
		if (!useThreads)
		{
			for (int i = 0; i < m_componentData.size(); i++)
			{
				if(!m_componentData.at(i).m_bAvailableForReuse)
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

	void ComponentDatalist::ThreadUpdate(int _buffer, int _amount) override
	{
		int maxCap = _buffer + _amount;
		for (size_t co = _buffer; co < maxCap; co++)
		{
			if (co >= m_componentData.size())
				break;

			if (!m_componentData.at(co).m_bAvailableForReuse)
				m_componentData.at(co).OnUpdate();
		}
	}

	void ComponentDatalist::Render(bool useThreads, int maxComponentsPerThread) override
	{
		if (!useThreads)
		{
			for (int i = 0; i < m_componentData.size(); i++)
			{
				if (!m_componentData.at(i).m_bAvailableForReuse)
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

	void ComponentDatalist::ThreadRender(int _buffer, int _amount) override
	{
		int maxCap = _buffer + _amount;
		for (size_t co = _buffer; co < maxCap; co++)
		{
			if (co >= m_componentData.size())
				break;

			if (!m_componentData.at(co).m_bAvailableForReuse)
				m_componentData.at(co).OnRender();
		}
	}

	void ComponentDatalist::LoadComponentDataFromJson(nlohmann::json& j) override
	{
		for (auto it : j.items())
		{
			T component;
			component.OnInitialize();
			component.m_sEntityID = it.key();
			component.FromJson(j[it.key()]);

			component.AddComponent();
		}
	}

	nlohmann::json ComponentDatalist::WriteComponentDataToJson() override
	{
		nlohmann::json data;

		for (int i = 0; i < m_componentData.size(); i++)
		{
			if (m_componentData.at(i).m_bAvailableForReuse)
				continue;

			data[m_componentData.at(i).m_sEntityID] = m_componentData.at(i).WriteJson();
		}

		return data;
	}
};