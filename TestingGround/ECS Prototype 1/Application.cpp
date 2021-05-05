#include "Application.h"
#include "System.h"
#include "Entity.h"

#include <iostream>

std::vector<Entity> Application::entities;
std::vector<std::shared_ptr<SystemBase>> Application::systems;

void Application::InitializeApplication()
{
	std::cout << "Initializing Engine!" << std::endl;
}

Entity* Application::CreateEntity()
{
	entities.push_back(entities.size());
	return &entities.at(entities.size()-1);
}

void Application::MainLoop()
{
	while (true)
	{
		for (int i = 0; i < systems.size(); i++)
		{
			systems.at(i)->Update();
		}

		for (int i = 0; i < systems.size(); i++)
		{
			systems.at(i)->Render();
		}
	}
}