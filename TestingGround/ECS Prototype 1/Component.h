#pragma once
#include <iostream>
#include <vector>

class SystemBase;

struct ComponentBase
{
public:

	static std::weak_ptr<SystemBase> componentSystem;

	int entityID;

	static void AddToEntity(int ID);
};

template<typename T>
struct ComponentData : public ComponentBase
{
	static std::vector<T> componentData;

	static T* GetComponent(int _ID)
	{
		for (int i = 0; i < componentData.size(); i++)
		{
			if (componentData.at(i).entityID == _ID)
			{
				return &T::componentData.at(i);
			}
		}

		return nullptr;
	}
};

struct Transform : public ComponentData<Transform>
{
public:
	int x = 0, y = 0, z = 0;
	int xr = 0, yr = 0, zr = 0;
};
