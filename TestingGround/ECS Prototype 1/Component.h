#pragma once
#include <iostream>

class SystemBase;

struct Component
{
public:

	static std::weak_ptr<SystemBase> componentSystem;

	int entityID;

	static void AddToEntity(int ID);
};

struct Transform : public Component
{
public:
	int x = 0, y = 0, z = 0;
	int xr = 0, yr = 0, zr = 0;
};
