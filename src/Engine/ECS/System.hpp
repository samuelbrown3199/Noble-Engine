#pragma once

#include <memory>

#include <nlohmann/json.hpp>

#include "../Core/ThreadingManager.h"

//avoid using this in actual engine library compile so intel compiler can be used. Seems like its something to do with some inheritance
#define SetupComponent(T, U) \
std::weak_ptr<SystemBase> U::self; \
std::weak_ptr<SystemBase> T::componentSystem; \
std::vector<T> T::componentData; \

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
};

	template<typename T>
	/**
	* Inherit from this to create new systems.
	*/
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
		T::componentSystem = self;
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
				for (int i = 0; i < T::componentData.size(); i++)
				{
					OnUpdate(&T::componentData.at(i));
				}
			}
			else
			{
				double amountOfThreads = ceil(T::componentData.size() / maxComponentsPerThread) + 1;
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
			if (co >= T::componentData.size())
				break;

			OnUpdate(&T::componentData.at(co));
		}
	}
	/**
	*Loops through the component data and renders them.
	*/
	void Render()
	{
		if (systemUsage == useRender || systemUsage == useBoth)
		{
			for (int i = 0; i < T::componentData.size(); i++)
			{
				OnRender(&T::componentData.at(i));
			}
		}
	}

	void RemoveComponent(std::string _ID)
	{
		for (int i = 0; i < T::componentData.size(); i++)
		{
			if (T::componentData.at(i).m_sEntityID == _ID)
			{
				T::componentData.erase(T::componentData.begin() + i);
				break;
			}
		}
	}

	void RemoveAllComponents()
	{
		for (int i = 0; i < T::componentData.size(); i++)
		{
			T::componentData.at(i).OnRemove();
		}

		T::componentData.clear();
	}

protected:
	/**
	* Overwrite this to create functionality on a component every update.
	*/
	virtual void OnUpdate(T* comp) {};
	/**
	* Overwrite this to create functionality on a component every rendering process.
	*/
	virtual void OnRender(T* comp) {};

public:

	int GetComponentIndex(std::string _ID)
	{
		return T::GetComponentIndex(_ID);
	}

	T* GetComponent(std::string _ID)
	{
		return T::GetComponent(_ID);
	}

	void LoadComponentDataFromJson(nlohmann::json& j)
	{
		for (auto it : j.items())
		{
			T component;
			component.m_sEntityID = it.key();
			component.FromJson(j[it.key()]);

			T::componentData.push_back(component);
		}
	}

	nlohmann::json WriteComponentDataToJson()
	{
		nlohmann::json data;

		for (int i = 0; i < T::componentData.size(); i++)
		{
			data[T::componentData.at(i).m_sEntityID] = T::componentData.at(i).WriteJson();
		}

		return data;
	}

};