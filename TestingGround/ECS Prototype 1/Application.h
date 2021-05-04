#pragma once
#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <iostream>
#include <vector>
#include <memory>

class SystemBase;

class Application
{
private:

public:

	static std::vector<int> entities;
	static std::vector<std::shared_ptr<SystemBase>> systems;

	static void InitializeApplication();

	void CreateEntity();

	void MainLoop();

	template<typename T>
	std::shared_ptr<T> BindSystem()
	{
		std::shared_ptr<T> temp;
		for (size_t sys = 0; sys < systems.size(); sys++)
		{
			temp = std::dynamic_pointer_cast<T>(systems.at(sys));
			if (temp)
			{
				std::cout << "System is already bound!!" << std::endl;
				return temp;
			}
		}

		std::shared_ptr<T> system = std::make_shared<T>();
		system->self = system;
		system->InitializeSystem();
		systems.push_back(system);
		return system;
	}
};

#endif

