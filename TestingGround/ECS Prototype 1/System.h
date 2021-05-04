#pragma once
#ifndef SYSTEM_H_
#define SYSTEM_H_

#include "Application.h"
#include "Component.h"

class SystemBase
{
public:
	virtual void Update() = 0;
	virtual void Render() = 0;

	virtual void OnUpdate() {};
	virtual void OnRender() {};
};

template<typename T>
class System : public SystemBase
{
public:
	std::vector<T> componentData;

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

	virtual void OnUpdate(T* comp) = 0;
	virtual void OnRender(T* comp) = 0;
};

class TransformSystem : public System<Transform>
{
	void OnUpdate(Transform* comp)
	{
		comp->x++;
		std::cout << "Transform component count: " << componentData.size() << std::endl;
		std::cout << "Transform Update! X: " << comp->x << std::endl;
	}

	void OnRender(Transform* comp)
	{
		std::cout << "Transform Render!" << std::endl;
	}
};

#endif

