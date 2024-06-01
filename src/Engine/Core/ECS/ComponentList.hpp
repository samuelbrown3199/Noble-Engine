#pragma once

#include <nlohmann/json.hpp>

#include <vector>
#include <deque>

#include "../../Core/Application.h"
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
	virtual void PreRender(bool useThreads, int maxComponentsPerThread) = 0;
	virtual void ThreadPreRender(int _buffer, int _amount) = 0;
	virtual void LoadComponentDataFromJson(nlohmann::json& j) = 0;
	virtual nlohmann::json WriteComponentDataToJson() = 0;

	virtual Component* CopyComponent(Component* comp, std::string entityID) = 0;
};

template <typename T>
struct ComponentDatalist : public Datalist
{
	std::vector<T> m_componentData; //could this be a unordered map for faster access?
	std::deque<T*> m_deletedComponents;

	void AddComponent(Component* comp) override
	{
		if (!m_deletedComponents.empty())
		{
			std::string entityID = comp->m_sEntityID;

			T* co = m_deletedComponents.front();
			*co = *dynamic_cast<T*>(comp);
			co->m_bAvailableForReuse = false;
			co->m_sEntityID = entityID;

			m_deletedComponents.pop_front();

			return;
		}

		m_componentData.push_back(*dynamic_cast<T*>(comp));
	}

	void AddComponentToEntity(std::string entityID) override
	{
		T comp;
		comp.m_sEntityID = entityID;
		comp.OnInitialize();
		comp.AddComponent();
	}

	void RemoveComponent(std::string entityID) override
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

	void RemoveAllComponents() override
	{
		m_componentData.clear();
		m_deletedComponents.clear();
	}

	int GetComponentIndex(std::string entityID) override
	{
		for (int i = 0; i < m_componentData.size(); i++)
		{
			if (m_componentData.at(i).m_sEntityID == entityID)
				return i;
		}

		return -1;
	}

	Component* GetComponent(int index) override
	{
		if (index > m_componentData.size() - 1 || index < 0)
			return nullptr;

		return &m_componentData.at(index);
	}

	void Update(bool useThreads, int maxComponentsPerThread) override
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
				auto th = Application::GetApplication()->GetThreadingManager()->EnqueueTask([&] { ThreadUpdate(buffer, maxComponentsPerThread); });
			}
		}
	}

	void ThreadUpdate(int _buffer, int _amount) override
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

	void PreRender(bool useThreads, int maxComponentsPerThread) override
	{
		if (!useThreads)
		{
			for (int i = 0; i < m_componentData.size(); i++)
			{
				if (!m_componentData.at(i).m_bAvailableForReuse)
					m_componentData.at(i).OnPreRender();
			}
		}
		else
		{
			double amountOfThreads = ceil(m_componentData.size() / maxComponentsPerThread) + 1;
			for (int i = 0; i < amountOfThreads; i++)
			{
				int buffer = maxComponentsPerThread * i;
				auto th = Application::GetApplication()->GetThreadingManager()->EnqueueTask([&] { ThreadPreRender(buffer, maxComponentsPerThread); });
			}
		}
	}

	void ThreadPreRender(int _buffer, int _amount) override
	{
		int maxCap = _buffer + _amount;
		for (size_t co = _buffer; co < maxCap; co++)
		{
			if (co >= m_componentData.size())
				break;

			if (!m_componentData.at(co).m_bAvailableForReuse)
				m_componentData.at(co).OnPreRender();
		}
	}

	void LoadComponentDataFromJson(nlohmann::json& j) override
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

	nlohmann::json WriteComponentDataToJson() override
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

	Component* CopyComponent(Component* comp, std::string entityID) override
	{
		T* newComp = new T();
		*newComp = *dynamic_cast<T*>(comp);
		newComp->m_sEntityID = entityID;

		AddComponent(newComp);

		return newComp;
	}
};

template<typename T>
struct ResourceCreator
{
	void AddResource(std::string path)
	{
		T* newResource = new T();
		newResource->m_sResourcePath = path;
		newResource->m_sLocalPath = GetFolderLocationRelativeToGameData(path) != "" ? GetFolderLocationRelativeToGameData(path) : path;

		Application::GetApplication()->GetResourceManager()->AddNewResource(newResource);
	}
};