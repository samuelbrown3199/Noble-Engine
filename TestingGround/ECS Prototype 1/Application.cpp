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

void Application::CreateEntity()
{
	entities.push_back(entities.size());
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