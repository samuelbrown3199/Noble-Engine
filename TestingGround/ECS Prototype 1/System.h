#pragma once
#ifndef SYSTEM_H_
#define SYSTEM_H_

#include "Application.h"
#include "Component.h"

#define SetupComponent(T, U) \
std::weak_ptr<SystemBase> U::self; \
std::weak_ptr<SystemBase> T::componentSystem; \


class SystemBase
{
public:
	static std::weak_ptr<SystemBase> self;

	virtual void Update() = 0;
	virtual void Render() = 0;

	virtual void OnUpdate() {};
	virtual void OnRender() {};

	virtual void AddComponent(int ID) = 0;
};

template<typename T>
class System : public SystemBase
{
public:
	std::vector<T> componentData;

	static void InitializeSystem()
	{
		T::componentSystem = self;
	}

	void AddComponent(int ID)
	{
		for (int i = 0; i < Application::entities.size(); i++)
		{
			if (Application::entities.at(i) == ID)
			{
				T comp;
				comp.entityID = ID;
				componentData.push_back(comp);

				std::cout << "Created component!" << std::endl;
				break;
			}
		}
	}

	void Update()
	{
		for (int i = 0; i < componentData.size(); i++)
		{
			OnUpdate(&componentData.at(i));
		}
	}
	void Render()
	{
		for (int i = 0; i < componentData.size(); i++)
		{
			OnRender(&componentData.at(i));
		}
	}

	virtual void OnUpdate(T* comp) {};
	virtual void OnRender(T* comp) {};
};

class TransformSystem : public System<Transform>
{
	void OnUpdate(Transform* comp)
	{
		comp->x++;
	}
};

#endif

