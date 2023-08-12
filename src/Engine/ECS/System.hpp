#pragma once

#include <memory>

#include <nlohmann/json.hpp>

#include "../Core/ThreadingManager.h"

struct Component;

enum SystemUsage
{
	useUpdate,
	useRender,
	useBoth
};
struct SystemBase
{
	std::string m_systemID;
	SystemUsage systemUsage;
	bool useThreads = false;
	/**
	*Determines how many components a thread should handle.
	*/
	int maxComponentsPerThread = 1024;

	virtual void PreUpdate() {};
	virtual void Update() = 0;
	virtual void PreRender() {};
	virtual void Render() = 0;

	virtual void RemoveComponent(std::string _ID) = 0;
	virtual void RemoveAllComponents() = 0;

	virtual int GetComponentIndex(std::string _ID) = 0;

	virtual void LoadComponentDataFromJson(nlohmann::json& j) = 0;
	virtual nlohmann::json WriteComponentDataToJson() = 0;

	virtual void AddComponent(Component* comp) = 0;
};

/**
* Inherit from this to create new systems.
*/
template<typename T>
class System : public SystemBase
{
	friend class Application;

private:
	/**
	*Keeps a weak pointer to itself.
	*/
	static std::weak_ptr<SystemBase> self;

	/**
	*Sets up the system.
	*/
	static void InitializeSystem(std::string _ID)
	{
		self.lock()->m_systemID = _ID;
		//NobleRegistry::RegisterComponent<T>(_ID);
	}
	/**
	*Loops through the component data and updates them.
	*/
	void Update()
	{
		if (systemUsage == useUpdate || systemUsage == useBoth)
		{
			if (!useThreads)
			{
				for (int i = 0; i < componentData.size(); i++)
				{
					OnUpdate(&componentData.at(i));
				}
			}
			else
			{
				double amountOfThreads = ceil(componentData.size() / maxComponentsPerThread) + 1;
				for (int i = 0; i < amountOfThreads; i++)
				{
					int buffer = maxComponentsPerThread * i;
					auto th = ThreadingManager::EnqueueTask([&] { ThreadUpdate(buffer, maxComponentsPerThread); });
				}
			}
		}
	}
	void ThreadUpdate(int _buffer, int _amount)
	{
		int maxCap = _buffer + _amount;
		for (size_t co = _buffer; co < maxCap; co++)
		{
			if (co >= componentData.size())
				break;

			OnUpdate(&componentData.at(co));
		}
	}
	/**
	*Loops through the component data and renders them.
	*/
	void Render()
	{
		if (systemUsage == useRender || systemUsage == useBoth)
		{
			for (int i = 0; i < componentData.size(); i++)
			{
				OnRender(&componentData.at(i));
			}
		}
	}

	void RemoveComponent(std::string _ID)
	{
		for (int i = 0; i < componentData.size(); i++)
		{
			if (componentData.at(i).m_sEntityID == _ID)
			{
				componentData.erase(componentData.begin() + i);
				break;
			}
		}
	}

	void RemoveAllComponents()
	{
		for (int i = 0; i < componentData.size(); i++)
		{
			componentData.at(i).OnRemove();
		}

		componentData.clear();
	}

protected:

	static std::vector<T> componentData;

	/**
	* Overwrite this to create functionality on a component every update.
	*/
	virtual void OnUpdate(T* comp) {};
	/**
	* Overwrite this to create functionality on a component every rendering process.
	*/
	virtual void OnRender(T* comp) {};

public:

	virtual void AddComponent(Component* comp) override
	{
		T* compT = dynamic_cast<T*>(comp);
		componentData.push_back(*compT);
	}

	int GetComponentIndex(std::string _ID)
	{
		for (int i = 0; i < componentData.size(); i++)
		{
			if (componentData.at(i).m_sEntityID == _ID)
				return i;
		}
		
		return -1;
	}

	static T* GetComponent(std::string _ID)
	{
		for (int i = 0; i < componentData.size(); i++)
		{
			if (componentData.at(i).m_sEntityID == _ID)
				return &componentData.at(i);
		}

		return nullptr;
	}

	void LoadComponentDataFromJson(nlohmann::json& j)
	{
		for (auto it : j.items())
		{
			T component;
			component.m_sEntityID = it.key();
			component.FromJson(j[it.key()]);

			componentData.push_back(component);
		}
	}

	nlohmann::json WriteComponentDataToJson()
	{
		nlohmann::json data;

		for (int i = 0; i < componentData.size(); i++)
		{
			data[componentData.at(i).m_sEntityID] = componentData.at(i).WriteJson();
		}

		return data;
	}

};