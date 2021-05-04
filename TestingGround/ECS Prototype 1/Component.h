#pragma once
#include <iostream>

struct Component
{
public:
	int entityID;
	virtual void Test()
	{
		std::cout << "Component!" << std::endl;
	}
};

struct Transform : public Component
{
public:
	int x = 0, y = 0, z = 0;
};
